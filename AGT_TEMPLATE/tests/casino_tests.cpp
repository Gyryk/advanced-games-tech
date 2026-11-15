#include "casino_game.h"
#include "casino_geometry.h"
#include <cstdlib>
#include <iostream>

namespace
{
    int assertions = 0;
    void expect(bool condition, const char* message)
    {
        ++assertions;
        if (!condition) { std::cerr << "FAIL: " << message << '\n'; std::exit(1); }
    }
    constexpr float dt = 1.f / 120.f;
    void advance(casino::game& g, int ticks, casino::controls input = {})
    {
        for (int i=0;i<ticks;++i) { g.step(dt,input); input.jump=false; }
    }
    casino::game isolated()
    {
        casino::game g; g.reset(); g.guards.clear(); return g;
    }
    void geometry_checks(const casino::geometry& g)
    {
        expect(g.indices.size()%3==0,"triangle list");
        for (const auto& v : g.vertices)
        {
            expect(std::abs(glm::length(v.normal)-1.f)<.0001f,"unit normals");
            expect(std::isfinite(v.uv.x)&&std::isfinite(v.uv.y),"finite UVs");
        }
        for (size_t i=0;i<g.indices.size();i+=3)
        {
            for (int j=0;j<3;++j) expect(g.indices[i+j]<g.vertices.size(),"valid mesh indices");
            const auto& a=g.vertices[g.indices[i]];
            const auto& b=g.vertices[g.indices[i+1]];
            const auto& c=g.vertices[g.indices[i+2]];
            const auto cross=glm::cross(b.position-a.position,c.position-a.position);
            expect(glm::length(cross)>.00001f,"no degenerate triangles");
            expect(glm::dot(cross,a.normal+b.normal+c.normal)>0.f,"outward winding matches normals");
        }
    }
}

int main()
{
    using namespace casino;
    game initial;
    expect(initial.state==round_state::intro,"starts at intro");
    advance(initial,120);
    expect(initial.remaining==game::round_seconds,"intro freezes game");
    initial.reset();
    expect(initial.guards.size()==4,"four NPCs");
    for (const auto& p:initial.pickups) expect(!initial.blocked(p.position,.4f),"all pickups accessible");
    for (const auto& g:initial.guards) for (auto p:g.route) expect(!initial.blocked(p,.4f),"patrol waypoints clear");

    auto g=isolated();
    g.player_position=g.pickups.front().position;
    advance(g,1); expect(g.score==50,"ordinary chip scores");
    advance(g,5); expect(g.score==50,"pickup only collected once");
    for (const auto& p:g.pickups) if (p.kind==pickup_kind::jackpot) g.player_position=p.position;
    advance(g,1); expect(g.alarm&&g.score==250,"jackpot triggers hard mode and bonus");
    for (const auto& p:g.pickups) if (p.kind==pickup_kind::security_pass) {g.player_position=p.position;break;}
    advance(g,1); expect(!g.alarm,"security pass restores easy mode");
    for (const auto& p:g.pickups) if (p.kind==pickup_kind::shield) {g.player_position=p.position;break;}
    advance(g,1); expect(g.shield_time>7.9f,"shield starts");
    g.toggle_pause(); const float shield=g.shield_time, time=g.remaining;
    advance(g,120); expect(g.remaining==time&&g.shield_time==shield,"pause freezes timer and powerup");
    g.toggle_pause(); advance(g,1000); expect(g.shield_time==0.f,"shield expires");

    g=isolated(); g.pickups.clear(); g.player_position={0,4};
    advance(g,60,{{1,0},false}); expect(g.player_velocity.x>4.f,"movement accelerates");
    const float speed=g.player_velocity.x; advance(g,30);
    expect(g.player_velocity.x<speed*.2f,"friction slows movement");
    g.player_position={0,4}; g.player_velocity={};
    advance(g,1,{{},true}); expect(g.player_height>0.f,"jump rises");
    advance(g,120); expect(g.player_height==0.f&&g.vertical_velocity==0.f,"gravity returns to ground");
    g.player_position={0,0}; g.player_velocity={};
    advance(g,600,{{1,0},false}); expect(g.player_position.x<1.1f,"crate blocks walking");
    g.player_position={.9f,0}; g.player_velocity={};
    advance(g,72,{{1,0},true}); expect(g.player_position.x>2.9f,"jump from crate edge clears it");
    advance(g,100); expect(!g.blocked(g.player_position,game::player_radius),"landing resolves solids");
    g.player_position={0,8}; g.player_velocity={};
    advance(g,600,{{1,0},false}); expect(g.player_position.x<=game::arena_x-game::player_radius,"boundary collision");

    g=isolated(); g.player_position=game::exit_position;
    advance(g,1); expect(g.state==round_state::playing,"exit requires target");
    // Collect every ordinary chip and return to exit: jackpot is optional.
    for (const auto& p:g.pickups) if(p.kind==pickup_kind::chip) {g.player_position=p.position;advance(g,1);}
    expect(g.score==game::target_score&&!g.alarm,"quiet route supplies target score");
    g.player_position=game::exit_position; advance(g,1);
    expect(g.state==round_state::escaped,"target plus exit wins");
    const auto won_position=g.player_position; advance(g,60,{{1,1},true});
    expect(length(g.player_position-won_position)==0.f,"finished round freezes movement");
    g.reset(); expect(g.score==0&&g.health==3&&!g.alarm&&g.pickups.front().active,"restart clears round state");
    g.guards.clear(); g.remaining=dt*.5f; advance(g,1);
    expect(g.state==round_state::timed_out,"timeout loses");

    g=isolated(); g.pickups.clear(); g.player_position={0,4};
    guard attacker; attacker.position={0,4.5f}; attacker.route={{{0,5},{1,5},{1,6},{0,6}}};
    attacker.state=guard_state::attack; attacker.state_time=.4f;
    g.guards={attacker}; advance(g,1);
    expect(g.health==2&&g.hit_flash>0.f,"guard contact damages with feedback");
    g.guards[0]=attacker; advance(g,1); expect(g.health==2,"damage cooldown prevents instant death");
    g.invulnerable_time=0.f; g.shield_time=3.f; g.guards[0]=attacker;
    advance(g,1); expect(g.health==2,"shield blocks guard damage");
    g.shield_time=0.f; g.health=1; g.player_position={0,4}; g.guards[0]=attacker;
    advance(g,1); expect(g.state==round_state::caught,"zero health loses");

    g=isolated(); g.pickups.clear(); g.player_position={0,4};
    attacker.position={0,4.8f}; attacker.state=guard_state::patrol; attacker.state_time=0.f;
    g.guards={attacker}; advance(g,1); expect(g.guards[0].state==guard_state::chase,"patrol detects player");
    advance(g,1); expect(g.guards[0].state==guard_state::attack,"chase enters attack");
    g.shield_time=8.f; advance(g,50); expect(g.guards[0].state==guard_state::returning,"attack enters recovery");
    g.player_position={-10,0}; advance(g,600); expect(g.guards[0].state==guard_state::patrol,"recovery returns to patrol");

    geometry_checks(box_geometry()); geometry_checks(cylinder_geometry());
    geometry_checks(pyramid_geometry()); geometry_checks(floor_geometry());
    std::cout<<"Passed "<<assertions<<" checks (game rules, collision, AI, mesh normals/UVs/winding).\n";
}
