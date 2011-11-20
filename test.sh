echo *** Obligatory ***

echo
echo "ls"
ls

echo
echo "ls -l"
ls -l

echo
echo *pipeline*
ls | grep .txt

echo
echo *redirect stdout*
echo redirect stdout > prueba.txt
cat prueba.txt
rm prueba.txt

echo
echo *redirect stdin*
echo redirect stdin > prueba.txt
cat < prueba.txt
rm prueba.txt


echo
echo
echo *** Optional I ***

echo
echo *heredoc*
cat [
una y
otra lin.
]

echo
echo *heredoc pipeline*
cat | wc -l [
una y
otra lin.
]


echo
echo
echo *** Optional II ***

cmd = ls
arg = /tmp
$cmd $arg


echo
echo
echo *** Optional III ***

a % ls -l
echo $a

echo
a % ls | grep .txt
echo $a