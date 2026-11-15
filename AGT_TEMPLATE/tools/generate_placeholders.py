"""Generate original casino placeholders using Python's standard library only.
Run once with Python 3; generated files are already supplied with the game.
Use --output for experiments. --force deliberately replaces existing placeholders.
"""
from pathlib import Path
import argparse
import math
import random
import struct
import wave
import zlib


def png(path, width, height, pixel):
    def chunk(kind, data):
        return struct.pack('>I', len(data)) + kind + data + struct.pack('>I', zlib.crc32(kind + data) & 0xffffffff)
    rows = b''.join(b'\0' + bytes(c for x in range(width) for c in pixel(x, y)) for y in range(height))
    path.write_bytes(b'\x89PNG\r\n\x1a\n' + chunk(b'IHDR', struct.pack('>IIBBBBB', width, height, 8, 2, 0, 0, 0)) + chunk(b'IDAT', zlib.compress(rows)) + chunk(b'IEND', b''))


FONT = {
    'C': ['01111','10000','10000','10000','10000','10000','01111'],
    'A': ['01110','10001','10001','11111','10001','10001','10001'],
    'S': ['01111','10000','10000','01110','00001','00001','11110'],
    'I': ['11111','00100','00100','00100','00100','00100','11111'],
    'N': ['10001','11001','11001','10101','10011','10011','10001'],
    'O': ['01110','10001','10001','10001','10001','10001','01110'],
    'E': ['11111','10000','10000','11110','10000','10000','11111'],
    'X': ['10001','10001','01010','00100','01010','10001','10001'],
    'T': ['11111','00100','00100','00100','00100','00100','00100'],
}


def sign(path, word, foreground):
    w, h, scale = 512, 128, 10
    start = (w - (len(word) * 6 - 1) * scale) // 2
    def pixel(x, y):
        if x < 4 or x >= w - 4 or y < 4 or y >= h - 4:
            return foreground
        cx, cy = (x - start) // scale, (y - 29) // scale
        if 0 <= cx < len(word) * 6 and 0 <= cy < 7 and cx % 6 < 5:
            if FONT[word[cx // 6]][cy][cx % 6] == '1':
                return foreground
        return (18, 21, 31)
    png(path, w, h, pixel)


FACES = [
    ((0,0,1), [(-1,-1,1),(1,-1,1),(1,1,1),(-1,1,1)]),
    ((0,0,-1), [(1,-1,-1),(-1,-1,-1),(-1,1,-1),(1,1,-1)]),
    ((1,0,0), [(1,-1,1),(1,-1,-1),(1,1,-1),(1,1,1)]),
    ((-1,0,0), [(-1,-1,-1),(-1,-1,1),(-1,1,1),(-1,1,-1)]),
    ((0,1,0), [(-1,1,1),(1,1,1),(1,1,-1),(-1,1,-1)]),
    ((0,-1,0), [(-1,-1,-1),(1,-1,-1),(1,-1,1),(-1,-1,1)]),
]


def obj(path, parts, texture):
    lines = ['# Original generated placeholder; replace through casino_assets.h', f'mtllib {path.stem}.mtl', 'usemtl surface']
    index = 1
    for centre, size in parts:
        for normal, corners in FACES:
            for corner, uv in zip(corners, [(0,0),(1,0),(1,1),(0,1)]):
                position = [centre[i] + corner[i] * size[i] * .5 for i in range(3)]
                lines += ['v ' + ' '.join(map(str, position)), f'vt {uv[0]} {uv[1]}', 'vn ' + ' '.join(map(str, normal))]
            for tri in [(0,1,2),(0,2,3)]:
                lines.append('f ' + ' '.join(f'{index+i}/{index+i}/{index+i}' for i in tri))
            index += 4
    path.write_text('\n'.join(lines) + '\n')
    path.with_suffix('.mtl').write_text(f'newmtl surface\nKa 1 1 1\nKd 1 1 1\nKs 0.1 0.1 0.1\nNs 16\nmap_Kd ../textures/{texture}.png\n')


def sound(path, notes, seconds, volume=.18):
    rate = 22050
    samples = []
    segment = seconds / len(notes)
    for i in range(int(rate * seconds)):
        t = i / rate
        note = min(int(t / segment), len(notes) - 1)
        local = t - note * segment
        envelope = min(1., local / .01, (segment - local) / .03)
        frequency = notes[note]
        value = math.sin(2 * math.pi * frequency * t) + .22 * math.sin(4 * math.pi * frequency * t)
        samples.append(struct.pack('<h', int(32767 * volume * max(0, envelope) * value)))
    with wave.open(str(path), 'wb') as output:
        output.setparams((1, 2, rate, 0, 'NONE', 'not compressed'))
        output.writeframes(b''.join(samples))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output', type=Path, default=Path(__file__).resolve().parents[1] / 'game/assets/casino')
    parser.add_argument('--force', action='store_true')
    args = parser.parse_args()
    root = args.output
    if root.exists() and any(root.rglob('*.png')) and not args.force:
        parser.error('Assets already exist. Use a different --output, or --force only to replace them.')
    for folder in ('models', 'textures', 'audio'):
        (root / folder).mkdir(parents=True, exist_ok=True)
    textures = root / 'textures'
    rng = random.Random(17)
    png(textures/'paving.png',128,128,lambda x,y: (45,49,59) if x % 32 < 2 or y % 32 < 2 else ((115,118,126) if (x//32+y//32)%2 else (95,98,108)))
    png(textures/'felt.png',128,128,lambda x,y: (18,90+rng.randrange(12),57))
    png(textures/'metal.png',128,128,lambda x,y: (160+y%4*4,166+y%4*4,180+y%4*4))
    png(textures/'wood.png',128,128,lambda x,y: (100+x%9*3,55+x%9*2,35+x%9))
    png(textures/'stone.png',128,128,lambda x,y: (102+x%7,109+x%7,112+x%7))
    png(textures/'crate.png',128,128,lambda x,y: (45,35,28) if x%32<4 or y%32<4 else (155,117,73))
    def chip(x,y):
        radius = math.hypot(x-63.5,y-63.5)
        angle = math.atan2(y-63.5,x-63.5)
        if 42 < radius < 59 and int((angle+math.pi)*6/math.pi)%2 == 0: return (255,255,255)
        if 32 < radius < 35: return (240,240,240)
        return (168,168,168)
    png(textures/'chip.png',128,128,chip)
    sign(textures/'casino.png','CASINO',(255,202,97))
    sign(textures/'exit.png','EXIT',(220,255,232))
    # Matching solid-colour faces are intentionally plain but mathematically seamless.
    for face in ('front','right','back','left','top','bottom'):
        png(textures/f'sky_{face}.png',32,32,lambda x,y:(10,16,34))
    chair = [((0,.65,0),(.8,.16,.8)),((0,1.05,-.34),(.8,.75,.12))]
    chair += [((x,.3,z),(.12,.6,.12)) for x in (-.3,.3) for z in (-.3,.3)]
    obj(root/'models/chair.obj',chair,'wood')
    obj(root/'models/lamp.obj',[((0,.1,0),(.6,.2,.6)),((0,1.25,0),(.13,2.4,.13)),((0,2.5,0),(.65,.5,.65)),((0,2.81,0),(.8,.12,.8))],'metal')
    obj(root/'models/planter.obj',[((0,.35,0),(.9,.7,.9)),((0,.72,0),(1,.1,1)),((0,1.,0),(.5,.5,.5)),((.22,1.23,.1),(.35,.35,.35)),((-.22,1.15,-.1),(.4,.4,.4))],'stone')
    for name,notes,duration in [
        ('collect',[660,990],.18), ('alarm',[330,660,330,660],.8),
        ('powerup',[440,660,880],.45), ('damage',[150,90],.28),
        ('win',[523,659,784,1046],.8), ('lose',[330,220,110],.8)]:
        sound(root/'audio'/f'{name}.wav',notes,duration)
    sound(root/'audio/ambience.wav',[110,130.81,146.83,130.81],8.,.08)
    print(f'Generated {len(list(root.rglob("*.*")))} original placeholder files in {root}')


if __name__ == '__main__':
    main()
