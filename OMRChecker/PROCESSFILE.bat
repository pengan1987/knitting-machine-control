copy %1 D:\DotTextTile\ScanBackup
d:
cd D:\DotTextTile\OMRChecker
magick mogrify -gravity South -crop x600+0+0 %1
python main.py -f %1

del %1
