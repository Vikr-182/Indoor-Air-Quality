count=0

while true; do
    echo Running script
    python3 h.py >> sample.csv
    sleep 2
    (( count++ ))
    echo "Ran $count times"
done 
