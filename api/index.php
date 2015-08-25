<?php
	// Single Sign-On Example API
	// (C) 2013 CubicleSoft.  All Rights Reserved.

	require_once "../../client/config.php";
	require_once SSO_CLIENT_ROOT_PATH . "/index.php";

	define("API_VER", 1);

	function OutputError($msg, $code)
	{
		$result = array(
			"ver" => API_VER,
			"success" => false,
			"error" => $msg,
			"errorcode" => $code
		);

		echo json_encode($result);
		exit();
	}

	if (!SSO_LoggedIn())  OutputError("Not logged in.", "no_login");

	if (isset($_REQUEST["action"]) && $_REQUEST["action"] == "logout")
	{
		SSO_Logout();

		$result = array(
			"ver" => API_VER,
			"success" => true,
			"msg" => "Successfully logged out."
		);

		echo json_encode($result);
	}
	else if (isset($_REQUEST["action"]) && $_REQUEST["action"] == "test")
	{
		$randmsg = array(
			"Help someone every day.",
			"Maintain a positive personality.",
			"Enjoy life.",
			"Work before play.",
			"Smile.  Laugh.  Watch a cat video.",
			"Treat everyone with respect.",
			"Exercise a little each day.",
			"Make a few lifelong friends.",
			"Life is a gift.  Unwrap it gently."
		);

		$num = mt_rand(0, count($randmsg));
		if ($num == count($randmsg))  OutputError("Random error.  This test API has a random 1 in " . (count($randmsg) + 1) . " chance of an 'error'.", "random_error");

		$result = array(
			"ver" => API_VER,
			"success" => true,
			"msg" => $randmsg[$num]
		);

		echo json_encode($result);
	}
	else
	{
		OutputError("Unknown API action.", "unknown_action");
	}
?>