<?php
function GetClientIp() {
  $ip = $_SERVER['REMOTE_ADDR'];
  if (isset($_SERVER['HTTP_CLIENT_IP']) && preg_match('/^([0-9]{1,3}\.){3}[0-9]{1,3}$/', $_SERVER['HTTP_CLIENT_IP'])) {
    $ip = $_SERVER['HTTP_CLIENT_IP'];
  } elseif(isset($_SERVER['HTTP_X_FORWARDED_FOR']) AND preg_match_all('#\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}#s', $_SERVER['HTTP_X_FORWARDED_FOR'], $matches)) {
    foreach ($matches[0] AS $xip) {
      if (!preg_match('#^(10|172\.16|192\.168)\.#', $xip)) {
        $ip = $xip;
        break;
      }
    }
  }
  return $ip;
}

function PrintVar($title, $arr) {
  if (sizeof($arr) == 0) {
    return;
  }
  echo '---------------------'.$title.'---------------------<br/>';
  foreach ($arr as $key => $value)
  {
    echo $key.': '.$value.'<br/>';
  }
  echo '---------------------'.$title.'---------------------<br/>';
}

$client_ip = GetClientIp();
echo 'client_ip: '.$client_ip.'<br/>';
PrintVar('server_info', $_SERVER);
PrintVar('header', getallheaders());
PrintVar('request', $_REQUEST);
?>