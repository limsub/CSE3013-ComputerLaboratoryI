###############################################

# Student ID: 20181294

################################################

src_dir="submit"
build_dir="build"
tc_dir="test_case"
log_dir="logs"
output_dir="output"
result_list="result.list"
num_of_test_case=3

################################################

# Don't modify the variables above             #
# Write your own program below the comment box #

################################################

# 디렉토리 초기화
rm -r $build_dir 2> /dev/null
rm -r $log_dir 2> /dev/null
rm -r $output_dir 2> /dev/null
rm $result_list 2> /dev/null

mkdir $build_dir
mkdir $log_dir
mkdir $output_dir
touch $result_list

# 파일 확장자 제대로 되어있는 애들만 실행파일 만들어서 build 폴더에 저장
for submit_file in $(ls $src_dir)
do	
	if [ "${submit_file##*.}" == 'c' ]
	then
		# 실행파일을 .out 형식으로 build 에 저장
		cd $src_dir
		gcc $submit_file -o "${submit_file%.*}.out" &> /dev/null
		if [ $? -eq 0 ]
		then
			mv "${submit_file%.*}.out" $cd../$build_dir
		else
			touch "${submit_file%.*}.log"
			gcc $submit_file -o "${submit_file%.*}.out" &> "${submit_file%.*}.log"
			mv "${submit_file%.*}.log" $cd../$log_dir
		fi
		cd ..
	else
		echo "Invalid file name $submit_file"
	fi
done

# build 폴더 들어가서 파일들(.out) 실행.
# input 파일 3개(in_1,2,3)에 따라 3개의 결과 파일(20181294_1,2,3)  만들기
for build_file in $(ls $build_dir)
do	
	cd $build_dir

	for var in `eval echo {1..$num_of_test_case}`
	do	
		touch "${build_file%.*}_$var"
		cd ..
		cd $tc_dir
		num=`cat in_$var`
		cd ..
		cd $build_dir
		echo $num | ./${build_file} > "${build_file%.*}_$var"
		mv "${build_file%.*}_$var" $cd../$output_dir
	done

	cd ..
	cd $tc_dir
	touch "${build_file%.*}.log"
	cd ..
	cd $output_dir
	for var in `eval echo {1..$num_of_test_case}`
	do
		mv "${build_file%.*}_$var" $cd../$tc_dir
		cd ..
		cd $tc_dir
		echo "test case $var" >> "${build_file%.*}.log"		
		diff "${build_file%.*}_$var" "ans_$var" >> "${build_file%.*}.log"
		if [ $? -eq 0 ]
		then
			echo -e "pass\n" >> "${build_file%.*}.log"
		else
			echo -e "wrong anser\n" >> "${build_file%.*}.log"
		fi
		mv "${build_file%.*}_$var" $cd../$output_dir
		cd ..
		cd $output_dir
	done
	cd ..
	cd $tc_dir
	mv "${build_file%.*}.log" $cd../$log_dir
	cd ..

done


cat students.list | while read line
do	
	

	cd $src_dir
	if [ ! -e "$line.c" ]
	then
		touch "$line.log"
		echo "not submitted" >> "$line.log"
		mv "$line.log" $cd../$log_dir
		score=0
		echo $line": not submitted"
		cd ..
	else
		cd ..
		cd $log_dir
		score=$(grep -o pass "$line.log" | wc -w)
		score=$(($score*2))

		if [ $(grep -o error "$line.log" | wc -w) == 1 ]
		then
			score=$(($score+1))
			echo $line": compile error"
		else
			score=$(($score+2))
			echo $line":" $score
		fi		
	
		cd ..
	fi
	echo $line $score >> $result_list
done




