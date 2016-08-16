import glob , os, subprocess ,sys
artist = "Ken Follett"
title = "Eye of the Needle"

indir = os.path.join("C:\\","book",title)  #C:\\book\A Distant Mirror"
if not os.path.exists(indir):
    os.makedirs(indir)
os.chdir(indir)
print("Saving files to ", indir )


scriptDirectory = os.path.dirname(os.path.realpath(__file__))
ripper = os.path.join(scriptDirectory, 'nlrip', 'nlrip.exe')

#RIP_CMD = "c:\\book\\CUETools_2.1.5\CUETools.ConsoleRipper.exe -D D:"
# cmd = '"C:\\book\\CUETools_2.1.5\\CUETools.ConsoleRipper.exe" --help'
# subprocess.call(cmd, shell=True)
# sys.exit()
#EJECT_CMD = '''powershell "(new-object -COM Shell.Application).NameSpace(17).ParseName('D:').InvokeVerb('Eject')"'''

# subprocess.call(EJECT_CMD, shell=True)
# sys.exit()

ripNum = 1

while False:
    subprocess.call("{} {:0>2}.wav".format(ripper, ripNum) , shell=True)
    # wave = glob.glob('*.wav')[-1]
    # print("imported file named", wave)
    # os.rename(os.path.join(indir,"cdimage.wav"), os.path.join(indir,"{:0>2}.wav".format(ripNum)))
    # subprocess.call(EJECT_CMD, shell=True)
    ans = input("Saved disk {}.  Rip another CD? [Y/n]".format(ripNum)) or 'Y'
    if ans not in ['Y','y']: 
        break
    ripNum  += 1




scriptDirectory = os.path.dirname(os.path.realpath(__file__))
encoder = os.path.join(scriptDirectory, 'qaac', 'qaac64.exe')

AAC_CMD = encoder + '  -o "{outdir}\\{filename}.m4b" -v32  --title "{title}" --artist "{artist}" --artwork "{art}" --concat "{files}"'



infiles = sorted( glob.glob(os.path.join( indir, "*.wav")) )

# print(infiles)


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




'''

"C:\Program Files (x86)\VideoLAN\VLC\vlc" -I http cdda:///D:/ --cdda-track=!x! :sout=#transcode{acodec=s161,channels=2}:std{access="file",mux=raw,dst="Track!x!.wav"} vlc://quit
#https://mutagen.readthedocs.io/en/latest/api/mp4.html


cat *.wav | faac -b 80 -P -X -o book.aac -

MP4Box -add book.aac book.m4b

mp4chaps --every 3600 book.m4b

mp4art --add driving_heat.jpeg driving_heat.m4b

mp4tags -album "Driving Heat" -artist "Richard Castle" driving_heat.m4b


# ..\qaac\qaac64.exe --concat -o test.m4b --title "Driving Heat" --artist "Richard Castle" --artwork driving_heat.jpeg *.wav
"C:\Program Files (x86)\VideoLAN\VLC\vlc" -I dummy cdda:///D:/ --sout-audio --sout "#transcode{acodec=s161,channels=2}:std{access=file,mux=wav,dst=01.wav"} vlc://quit



"\Program Files (x86)\freac\
freaccmd.exe -e WAVE -cd 0 -track all -d blah

freaccmd.exe -e WAVE  -o c:\book\testfreac.wav -cd 0 -track all
'''