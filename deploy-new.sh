# Build_Time and TOKEN must be provided in build.sh
# Program depends it

date=$(date)
token="B8oXaAifqOPw+Sv6NexjzSqmuhw6a2DATWdjCh942R8="
echo 'char *TOKEN = "'$token'";char *Build_Time = "'$date'";' > config.h
echo '' >> config.h

# build the dockerfile
docker build -t sowmiyankarf/cpy-e:latest .

echo "done!"
