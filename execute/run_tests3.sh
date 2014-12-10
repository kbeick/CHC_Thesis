 #!/bin/sh

# say "go home Derek! you're bugging the crap outta me! Gosh!"

> log_data_output_-_3.txt
filename=$'list_tests3.txt'

make

while read -r line
do
    [ "${line::1}" = "#" ] && continue

    echo ""
    
    $line
    OUT=$?
    if [ $OUT -eq 0 ];then
        ed -s log_data_output_-_3.txt << "EOF"
        H
        a
...................................
.
        w
EOF
        sleep 0.2
        # say "SUCCESS!"
    else  
        ed -s log_data_output_-_3.txt << "EOF"
        H
        a
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
.
        w
EOF
        sleep 0.2
        say "FAILED!"
    fi

done < $filename

echo "
:..........................................:"
say "all done"
