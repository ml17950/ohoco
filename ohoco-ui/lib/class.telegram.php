<?php	// last change: 2018-02-20
class clsTelegram {
	var $debug;
	var $api_id;
	var $api_hash;
	var $api_key;
	var $api_url;

	function __construct($api_key) {
		$this->debug = false;
		$this->api_key = $api_key;
		$this->api_url = 'https://api.telegram.org/bot'.$this->api_key.'/';
	}

	function __destruct() {
	}

	function sendPostRequest($url, $headers = '', $data = '') {
		$ch = curl_init($url);
		curl_setopt($ch, CURLOPT_POST, 1);
		curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
		curl_setopt($ch, CURLOPT_HTTPHEADER, $headers);
		curl_setopt($ch, CURLOPT_POSTFIELDS, json_encode($data));
		$response = curl_exec($ch);
// 		if ($this->debug) echo "Response: ".$response."<hr>";
		curl_close($ch);

		return $response;
	}

	function getMe() {
		$url = $this->api_url.'getMe';
		$headers = array('Content-Type: application/json');
		$postData = array();
		$response = $this->sendPostRequest($url, $headers, $postData);
		if ($this->debug) echo "Response: ",highlight_string(print_r(json_decode($response), true)),"<hr>";
	}

	function getUpdates() {
		$url = $this->api_url.'getUpdates';
		$headers = array('Content-Type: application/json');
		$postData = array();
		$response = $this->sendPostRequest($url, $headers, $postData);
		if ($this->debug) echo "Response: ",highlight_string(print_r(json_decode($response), true)),"<hr>";
	}

	function sendMessage($chat_id, $message) {
		$url = $this->api_url.'sendMessage';
		$headers = array('Content-Type: application/json');
		$postData = array('chat_id' => $chat_id, 'text' => $message, 'parse_mode' => 'Markdown');
		$response = $this->sendPostRequest($url, $headers, $postData);
		if ($this->debug) echo "Response: ",highlight_string(print_r(json_decode($response), true)),"<hr>";
	}
}
?>