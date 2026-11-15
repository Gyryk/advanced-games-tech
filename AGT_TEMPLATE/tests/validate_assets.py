"""Check shipped placeholder paths and file structure without an OpenGL context."""
from pathlib import Path
import math
import re
import struct
import wave
import zlib

root = Path(__file__).resolve().parents[1]
assets = root / 'game/assets/casino'
header = (root / 'game/src/casino_assets.h').read_text()
paths = re.findall(r'"(assets/[^"\n]+)"', header)
for path in paths:
    assert (root / 'game' / path).is_file(), f'Missing asset: {path}'

for path in (assets / 'textures').glob('*.png'):
    data = path.read_bytes()
    assert data[:8] == b'\x89PNG\r\n\x1a\n'
    offset, payload, width, height = 8, b'', 0, 0
    while offset < len(data):
        size = struct.unpack('>I', data[offset:offset+4])[0]
        kind = data[offset+4:offset+8]
        chunk = data[offset+8:offset+8+size]
        crc = struct.unpack('>I', data[offset+8+size:offset+12+size])[0]
        assert zlib.crc32(kind+chunk) & 0xffffffff == crc, f'PNG CRC: {path}'
        if kind == b'IHDR': width, height = struct.unpack('>II', chunk[:8])
        if kind == b'IDAT': payload += chunk
        offset += size + 12
    assert len(zlib.decompress(payload)) == height * (1 + width * 3), path
sky = [(assets / 'textures' / f'sky_{face}.png').read_bytes() for face in ('front','right','back','left','top','bottom')]
assert all(image == sky[0] for image in sky), 'Placeholder sky faces must match'

triangles = 0
for path in (assets / 'models').glob('*.obj'):
    vertices, texcoords, normals = [], [], []
    for line in path.read_text().splitlines():
        fields = line.split()
        if not fields: continue
        if fields[0] == 'mtllib': assert (path.parent / fields[1]).is_file()
        if fields[0] == 'v': vertices.append(tuple(map(float, fields[1:])))
        if fields[0] == 'vt': texcoords.append(tuple(map(float, fields[1:])))
        if fields[0] == 'vn': normals.append(tuple(map(float, fields[1:])))
        if fields[0] == 'f':
            assert len(fields) == 4
            corners = [tuple(int(v)-1 for v in field.split('/')) for field in fields[1:]]
            for v,t,n in corners:
                assert 0 <= v < len(vertices) and 0 <= t < len(texcoords) and 0 <= n < len(normals)
                assert abs(sum(c*c for c in normals[n])-1.) < 1e-6
            a,b,c = [vertices[corner[0]] for corner in corners]
            ab,ac = [b[i]-a[i] for i in range(3)], [c[i]-a[i] for i in range(3)]
            cross = [ab[1]*ac[2]-ab[2]*ac[1], ab[2]*ac[0]-ab[0]*ac[2], ab[0]*ac[1]-ab[1]*ac[0]]
            assert sum(cross[i]*normals[corners[0][2]][i] for i in range(3)) > 0, f'Winding: {path}'
            triangles += 1
    for line in path.with_suffix('.mtl').read_text().splitlines():
        if line.startswith('map_Kd '): assert (path.parent / line[7:]).is_file()

for path in (assets / 'audio').glob('*.wav'):
    with wave.open(str(path)) as audio:
        assert audio.getnchannels() == 1 and audio.getsampwidth() == 2
        assert audio.getframerate() == 22050 and audio.getnframes() > 0
        frames = audio.readframes(audio.getnframes())
        samples = struct.unpack('<' + 'h' * (len(frames)//2), frames)
        assert 0 < max(abs(sample) for sample in samples) < 32767

print(f'Validated {len(paths)} configured paths, PNG CRCs/sky seams, {triangles} imported triangles, MTL texture links and WAV data.')
