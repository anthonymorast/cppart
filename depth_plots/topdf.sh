for i in *.eps; do
    [ -f "$i" ] || break
    filename="${i%.*}"
    filename="$filename.pdf"
    inkscape --export-pdf=$filename $i
done
