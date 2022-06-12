<?php
	session_start();
	$_SESSION["test_session"] = "Hello World";
	setcookie("test_cookie", "just a random cookie");
?>