<?php
error_reporting(0);
ini_set('display_errors', 0);
/*
header('X-Frame-Options: SAMEORIGIN');
header('X-XSS-Protection: 1; mode=block');
header('X-Content-Type-Options: nosniff');
*/

function get_txt_content ($local) {

	$file_lines = file("$local");

	foreach($file_lines as $tmp ) { 
		$content.=$tmp; 
	}

	return $content;

}


?>
