#!/bin/bash
if [ -e output ]; then
	  rm output -rf
fi

cwd=$(pwd)
echo "CWD: ${cwd}"
mkdir output
for d in $(ls); do
	  if [ -d $d ]; then
        if [ x"$d" = x"output" ]; then
            continue
        fi
        if [ x"$d" = x"lib" ]; then
            continue
        fi
        echo "mkdir output/${d}"
		    mkdir output/${d}
		    pushd $d
		    for f in $(ls); do
			      name=$(basename -- "$f")
			      ext="${name##*.}"
			      if [ x"${ext}" = x"html" ]; then
				        echo "Remove ${f}"
				        rm $f
            else
			          cat ${f} | mandoc -Thtml | python3 ${cwd}/lib/mklinks.py -p /man/ -o ${cwd}/output/$d/$f.html
			          echo "Processed ${d}/${f}"
            fi
		    done
        rm ${cwd}/output/$d/INDEX.html
        python3 ${cwd}/lib/mksectionindex.py /man/ ${d} > ${cwd}/output/$d/index.html
		    popd
	  fi
done
cp ${cwd}/index.html output
