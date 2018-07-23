<?php
$access_token = 'n2ZL8rR+WXoNKH/4JVVKZakNjuNDppWd4s+hN2VagjW/6wLdya9IE/r27Z0NXyD5iOgwi0M/Aa12Bbov7EIsAEIx1p5teVAKC7LoXoBbi4gNym9Lr2MvYul3EQSQ+PPKB5SFr8yHGk7y7Jw8MW2DmwdB04t89/1O/w1cDnyilFU=
';

$url = 'https://api.line.me/v1/oauth/verify';

$headers = array('Authorization: Bearer ' . $access_token);

$ch = curl_init($url);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
curl_setopt($ch, CURLOPT_HTTPHEADER, $headers);
curl_setopt($ch, CURLOPT_FOLLOWLOCATION, 1);
$result = curl_exec($ch);
curl_close($ch);

echo $result;
