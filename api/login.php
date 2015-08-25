<?php
	// Single Sign-On Application Login
	// (C) 2013 CubicleSoft.  All Rights Reserved.

	$sso_removekeys = array("sso_impersonate", "sso_remote_id");
	require_once "../../client/config.php";
	require_once SSO_CLIENT_ROOT_PATH . "/index.php";

	// Every starting request should wipe WebView session cookies since the API call session cookies are invalid.
	if (!isset($_REQUEST["action"]))
	{
		SSO_Logout();
		header("Location: " . SSO_GetFullRequestURLBase() . "?action=sso_start");

		exit();
	}

	$extra = array("app" => "1");
	if (isset($_REQUEST["sso_impersonate"]) && is_string($_REQUEST["sso_impersonate"]))  $extra["sso_impersonate"] = $_REQUEST["sso_impersonate"];
	else if (isset($_REQUEST["sso_remote_id"]) && is_string($_REQUEST["sso_remote_id"]))
	{
		$extra["sso_provider"] = "sso_remote";
		$extra["sso_remote_id"] = $_REQUEST["sso_remote_id"];
	}
	$lang = (isset($_REQUEST["lang"]) ? $_REQUEST["lang"] : "");
	$lang = preg_replace('/\s+/', "-", trim(preg_replace('/[^a-z]/', " ", strtolower($lang))));
	if (!SSO_LoggedIn())  SSO_Login($lang, "", $extra);

	// Send the browser cookies.
	SSO_SaveUserInfo();

	// Let the application know that the sign in is done.
	if ($_REQUEST["action"] === "sso_alt_done")
	{
		// For use with native applications that can't read browser cookies.
		// Most of this is anti-XSS/XSRF logic (e.g. same domain w/ iframe).

		$url = SSO_GetFullRequestURLBase();
?>
<!DOCTYPE html>
<html>
<head>
<title>SSO Client Done</title>
</head>
<body>
<script type="text/javascript">
try
{
	if (window.self !== window.top)  window.location.href = '<?php echo str_replace("'", "\\'", $url); ?>?action=sso_alt_done3';
	if (window.self !== window.parent)  window.location.href = '<?php echo str_replace("'", "\\'", $url); ?>?action=sso_alt_done3';
}
catch (ex)
{
}
</script>
<div style="text-align: center;">
<form id="sso_done" action="<?php echo htmlspecialchars($url); ?>" method="get">
	<input type="hidden" name="action" value="sso_alt_done2" />
	<input type="submit" value="Continue" />
</form>
</div>
<script type="text/javascript">
document.getElementById('sso_done').submit();
</script>
</body>
</html>
<?php
	}
	else if ($_REQUEST["action"] === "sso_alt_done2")
	{
		// Really ancient web browsers can spoof HTTP_REFERER, but those browsers have other major security vulnerabilities.
		if ($_SERVER["HTTP_REFERER"] === SSO_GetFullRequestURLBase() . "?action=sso_alt_done")
		{
			$result = array("success" => true, "msg" => "You are signed in.  This message should never really be seen by a user.");
			$result["br"] = str_repeat("<br />", 100);
			$result["name"] = SSO_COOKIE_NAME;
			$domain = SSO_GetRequestHost();
			$pos = strpos($domain, "://");
			$domain = substr($domain, $pos + 3);
			$pos = strpos($domain, ":");
			if ($pos !== false)  $domain = substr($domain, 0, $pos);
			$result["domain"] = $domain;
			$result["path"] = SSO_COOKIE_PATH;
			$result["ssl"] = SSO_COOKIE_SSL_ONLY;
			$result["c"] = $_COOKIE[SSO_COOKIE_NAME . "_c"];
			$result["s"] = $_COOKIE[SSO_COOKIE_NAME . "_s"];
			$result["st"] = (SSO_COOKIE_TIMEOUT > 0 ? time() + SSO_COOKIE_TIMEOUT : 0);
			$result["v"] = $_COOKIE[SSO_COOKIE_NAME . "_v"];
			$result["vt"] = (SSO_COOKIE_TIMEOUT > 0 && !SSO_COOKIE_EXIT_TIMEOUT ? time() + SSO_COOKIE_TIMEOUT : 0);

			echo json_encode($result);
		}
		else
		{
			echo json_encode(array("success" => false, "error" => "You are signed in.  Unfortunately, the application has HTTP referrers disabled so it will be unable to continue.", "errorcode" => "invalid_http_referer"));
		}
	}
	else if ($_REQUEST["action"] === "sso_alt_done3")
	{
		// Catch apps/users that attempt to iframe 'sso_alt_done'.
		echo json_encode(array("success" => false, "error" => "You are signed in.  Unfortunately, the application has made a request that would result in exposure of session information in an insecure way.", "errorcode" => "invalid_access"));
	}
	else if ($_REQUEST["action"] !== "sso_done")  header("Location: " . SSO_GetFullRequestURLBase() . "?action=sso_done");
	else
	{
		// Extracting browser cookies is the preferred method.
		echo json_encode(array("success" => true, "msg" => "You are signed in.  This message should never really be seen by a user."));
	}
?>