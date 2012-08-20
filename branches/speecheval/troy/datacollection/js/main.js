function validatePass(p1, p2) {
	if (p1.value != p2.value || p1.value == '' || p2.value == '') {
		p2.setCustomValidity('Password mismatch!');
	}else {
		p2.setCustomValidity('');
	}
}


