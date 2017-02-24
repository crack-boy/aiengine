<?php
include 'Aiengine.php';
$skegn=new Aiengine();

$score=$skegn->aiengineResule('Hello, I\'m Mike.', dirname(__FILE__)."/test.mp3","100",0,'en.sent.score');

echo $score;