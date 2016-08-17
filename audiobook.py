import glob , os, subprocess ,sys
from metadata import * 

scriptDirectory = os.path.dirname(os.path.realpath(__file__))


indir = os.path.join("C:\\","book",title)  #C:\\book\A Distant Mirror"
if not os.path.exists(indir):
    os.makedirs(indir)
# print("Saving files to: ", indir )
os.chdir(indir)
# print("Local directory: ", os.getcwd() )

RIP_EXE = 'nlrip.exe'
ripper = None
for root, dirs, files in os.walk(scriptDirectory):
        if RIP_EXE in files:
            ripper = os.path.join(root, RIP_EXE)

print("Ripper:", ripper)

ripNum = 1
while True:
    subprocess.call("{} {:0>2}.wav".format(ripper, ripNum) , shell=True)
    ans = input("Saved disk {}.  Rip another CD? [Y/n]".format(ripNum)) or 'Y'
    if ans not in ['Y','y']: 
        break
    ripNum  += 1

encoder = os.path.join(scriptDirectory, 'qaac', 'qaac64.exe')
AAC_CMD = encoder + '  -o "{outdir}\\{filename}.m4b" -v32  --title "{title}" --artist "{artist}" --artwork "{art}" --concat "{files}"'



infiles = sorted( glob.glob(os.path.join( indir, "*.wav")) )

artwork = glob.glob(os.path.join( indir, "*.jp*" ) )[0]

# Break up large audibooks for iPod compatibility (13 hour max)
n = 12
if len(infiles) > n:
    part = 1
else:
    part = 0


for i in range(0,len(infiles),n):
    files = '" "'.join( infiles[i:i+n]  )
    if part:
        filename = '{} Part {}'.format(title, part)
    else:
        filename = title
    print('-----------------------------')
    cmd = AAC_CMD.format(outdir = indir, filename = filename, artist = artist, title = title, art = artwork, files = files)
    print(cmd)
    sts = subprocess.call(cmd, shell=True)
    print('===============================')

    part += 1



ans = input("Encoding done.  Press Enter to continue.")
