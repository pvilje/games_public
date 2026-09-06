from PIL import Image
import os

root = os.path.dirname(os.path.dirname(__file__))
src = os.path.join(root, 'assets', 'logo.png')
dst = os.path.join(root, 'assets', 'logo.ico')

if not os.path.exists(src):
    print('source not found:', src)
    raise SystemExit(1)

img = Image.open(src).convert('RGBA')
# Save multiple sizes in ICO
sizes = [(256,256),(128,128),(64,64),(32,32),(16,16)]
img.save(dst, format='ICO', sizes=sizes)
print('wrote', dst)
