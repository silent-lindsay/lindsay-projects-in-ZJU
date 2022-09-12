if (self == top) {
	$(function() {
		var bfJsUrl = $('#barrierfree').attr('src');
		bfJsUrl = bfJsUrl.substr(0, bfJsUrl.lastIndexOf('/') + 1) + 'js/';
		$.getScript(bfJsUrl + 'EasyReader.min.js', function() {
			$.getScript(bfJsUrl + 'jquery.cookie.js', function() {
				$.getScript(bfJsUrl + 'jquery.jplayer.min.js', function() {
					$.getScript(bfJsUrl + 'jquery.md5.min.js', function() {
						$.getScript(bfJsUrl + 'jsbrowser.js', function() {
							$.getScript(bfJsUrl + 'barrierfree.js');
						});
					});
				});
			});
		});
	});
}