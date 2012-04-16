PK4S=`ls *.pk4 | sed s/\.pk4//g`
echo ${PK4S}
for PK4 in ${PK4S}
do
	unzip -n ${PK4}.pk4 -d ${PK4}
done

