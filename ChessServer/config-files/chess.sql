DROP TABLE IF EXISTS chess_msg;
DROP TABLE IF EXISTS chess_user;

CREATE TABLE chess_user
(
	user_name CHAR(16) NOT NULL UNIQUE,
	user_pass CHAR(16) NOT NULL,
	PRIMARY KEY( user_name )
) CHARSET UTF8;

CREATE TABLE chess_msg
(
	msg_id	 INT UNSIGNED NOT NULL AUTO_INCREMENT,
	msg_from CHAR(16) NOT NULL,
	msg_text TEXT NOT NULL,
	PRIMARY KEY(msg_id),

	FOREIGN KEY(msg_from) REFERENCES chess_user(user_name) 
		ON DELETE CASCADE ON UPDATE CASCADE
) CHARSET UTF8;

DROP PROCEDURE IF EXISTS cup_Register;
DROP PROCEDURE IF EXISTS cup_Login;
DROP PROCEDURE IF EXISTS cup_SaveMsg;

DELIMITER //

CREATE PROCEDURE cup_Register($user_name CHAR(16), $user_pass CHAR(16) )
BEGIN
	DECLARE _res INT DEFAULT 0;
	DECLARE CONTINUE HANDLER FOR SQLEXCEPTION set _res = 1;

	INSERT INTO chess_user(user_name, user_pass) VALUES($user_name, $user_pass);
	SELECT _res 'RegisterResult' ;
END //

CREATE PROCEDURE cup_Login($user_name CHAR(16), $user_pass CHAR(16) )
BEGIN
	DECLARE _res INT DEFAULT 1;

	SELECT 0 FROM chess_user WHERE user_name = $user_name AND user_pass = $user_pass INTO _res;

	SELECT _res 'LoginResult';
END //

CREATE PROCEDURE cup_SaveMsg($msg_from CHAR(16), $msg_text TEXT)
BEGIN
	DECLARE _res INT DEFAULT 0;
	DECLARE CONTINUE HANDLER FOR SQLEXCEPTION set _res = 1;
	
	INSERT INTO chess_msg(msg_from, msg_text) VALUES($msg_from, $msg_text);

	SELECT _res 'SaveMsgResult';
END //

DELIMITER ;


