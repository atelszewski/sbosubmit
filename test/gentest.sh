#!/bin/sh

CWD=$(pwd)

if [ -e audio ]; then
  echo "audio already exists"
  exit 1
fi

mkdir -p audio/test-radio
cd audio/test-radio || exit 1

mkdir empty_dir

mkdir tmpdir
touch tmpdir/__01-THIS_WHOLE_DIRECTORY__
touch tmpdir/__02-SHOULD_BE_EXCLUDED__

mkdir tmpdirr
touch tmpdirr/__01-THIS_WHOLE_DIRECTORY__
touch tmpdirr/__02-SHOULD_BE_INCLUDED__

ln -s __01-THIS_WHOLE_DIRECTORY__ __LINK_SHOULD_BE_EXCLUDED__
mkfifo __FIFO_SHOULD_BE_EXCLUDED__

cat <<EOF > keywords.txt
,   ,,,key words, ,,
, multi
line, double comma ,, no space,comma,,,
EOF

cat <<EOF > comment.txt
I am a comment.
New line.
EOF

cat $CWD/gentest.in.SlackBuild > test-radio.SlackBuild
cat $CWD/gentest.in.slack-desc > slack-desc
cat $CWD/gentest.in.info       > test-radio.info
cat $CWD/gentest.in.README     > README
