declare -A timemap
timemap[1]="2:45:00"
timemap[2]="2:20:00"
timemap[4]="1:20:00"
timemap[8]="1:00:00"
timemap[16]="0:40:00"
timemap[20]="0:35:00"
timemap[32]="0:28:00"
timemap[48]="0:24:00"
timemap[64]="0:20:00"


for x in 16 20 32 48 64
do
echo "qsub -N 'mpi-fill$x' -pe mpi-fill $x -l h_rt=${timemap[$x]} sub.sh"
done;

for x in 2 4 8 16 20 32 48 64
do
echo "qsub -N 'mpi-spread$x' -pe mpi-spread $x -l h_rt=${timemap[$x]} sub.sh"
done;

for x in 16 32 64
do
echo "qsub -N 'mpi-8-$x' -pe mpi-8 $x -l h_rt=${timemap[$x]} sub.sh"
done;

for x in 4 8 16 32 64
do
echo "qsub -N 'mpi-4-$x' -pe mpi-4 $x -l h_rt=${timemap[$x]} sub.sh"
done;

for x in 2 4 8 16
do
echo "qsub -N 'mpi-$x-$x' -pe mpi-$x $x -l h_rt=${timemap[$x]} sub.sh"
done;
