<?php
class Query
{
	//const TYPE_LOGIN = 'login'; // ��� ������ ������ �� 1.0.8
	const TYPE_LOGIN = 'authorization';
	const TYPE_INSERT_USER = 'user_add';
	const TYPE_UPDATE_USER = 'user_update';
	const TYPE_DELETE_USER = 'user_delete';
	const TYPE_SELECT_USERS = 'select_users';
	const TYPE_AUTOPARK_NAMES = 'autopark_names';
	const TYPE_AUTOPARK_SET_NAME = 'park_set_name';
	const TYPE_MONTH_SET_DAY_VALUE = "month_set_day_value";
	const TYPE_DRIVERS_SELECT_DATA = "drivers_select_data";
	const TYPE_DRIVERS_SELECT_MONTH = "drivers_select_month";
	const TYPE_DRIVER_SELECT_MONTH = "driver_select_month";
	const TYPE_DRIVERS_SELECT_NODES = "drivers_select_nodes";
	const TYPE_DRIVER_SET_SHEDULE = "driver_set_schedule";
	const TYPE_DRIVER_SET_WORK = "driver_set_work";
	const TYPE_DRIVER_SET_NOT_WORK = "driver_set_not_work";
	const TYPE_DRIVER_ADD_NODE = "driver_add_node";
	const TYPE_DRIVER_EDIT_NODE = "driver_edit_node";
	const TYPE_DRIVER_REMOVE_NODE = "driver_remove_node";
	const TYPE_CARS_SELECT_DATA = "cars_select_data";
	const TYPE_CAR_SET_TIME_CHANGE_DRIVER = "car_set_time_change_driver";
	const TYPE_DRIVER_SET_STATE = "driver_set_state";
	const TYPE_DRIVER_NOT_RESPONDING = "update_drivers_not_responding";
	const TYPE_CANCELLATION_REPLACEMENT_DRIVER = "delete_and_update_cancellation_replacement_driver";
	const TYPE_TIME_TO_SLEEP = "update_time_to_sleep";
	const TYPE_AUTO_DAY_MONTH = "auto_day_month";
	const TYPE_AUTO_NEXT_MONTH = "auto_day_next_month";
	const TYPE_CREATE_TABLES_AUTOPARKS = "crate_tables_autoparks";
	const TYPE_INSERT_SUBSTITUTION = "insert_substitution";
	const TYPE_UPDATE_SUBSTITUTION = "update_substitution";
	const TYPE_SELECT_SUBSTITUTION = "select_substitution";
	const TYPE_DELETE_SUBSTITUTION = "delete_substitution";
	const TYPE_INSERT_SEQUENCE_DRIVERS = "insert_sequence";
	const TYPE_CLEAR_SEQUENCE_DRIVERS = "clear_sequence";
	const TYPE_CURRENT_VERSION = "current_version";
	const TYPE_SUBSTITUTIONS_DRIVER = "delete_substitutions_driver";
	const TYPE_DRIVER_INTERACESSION = "driver_intercession";
	const TYPE_SAVE_RADIAL_STATUS = "save_radial_status";
	const TYPE_STARTED_RECALL_PROGRAM = "get_started_program";
	const TYPE_SET_STARTED_RECALL_PROGRAM = "set_started_program";
	
	static function check_hash($hash, $str)
	{
		if($hash == md5($str))	
			return;
		print_data(array(ERR_TITLE => "hash",//Errors::HASH,
						  STATIS => "-4"));
	}
	
	static function connection()
	{
		try 
		{ 
			$pdo = new PDO("mysql:host=".Config::HOST.";dbname=".Config::NAME_DB, 
						   Config::USER_DB, 
						   Config::PASS_DB,
						   array(PDO::MYSQL_ATTR_INIT_COMMAND => "SET NAMES utf8")); 
			return $pdo;
		}
		catch(Exception $exc)
		{
			print_data(array(ERR_TITLE => Errors::CONNECTION_DB,
							  STATIS => "-4"));
		}
	}
	
	static function print_response($response)
	{
		$response[STATIS] = "1";
		echo json_encode($response, JSON_UNESCAPED_UNICODE | JSON_UNESCAPED_SLASHES);
		exit();
	}
	
	static function print_array_response($request)
	{
		$result = "";
		$response = '{"status":"1","array":[';
		while($result = $request->fetch(PDO::FETCH_ASSOC))
			$response .= json_encode($result, JSON_UNESCAPED_UNICODE | JSON_UNESCAPED_SLASHES).',';
		if($response == '{"status":"1","array":[')
			echo '{"status":"1"}';
		else
		{
			$response = substr($response, 0, strlen($response)-1);
			echo $response.']}';
		}
		exit();
	}

//	User	---------------------------------------------------------------------------------------------------------------
	public static function Login($assoc)
	{
		if(!isset($assoc['login']) ||
		   !isset($assoc['lpass']) ||
		   !isset($assoc['hash']))
		{
			print_data(array(ERR_TITLE => Errors::REQUETS,
							  STATIS => "-1"));
		}
		$login = urldecode($assoc['login']);
		$pass = urldecode($assoc['lpass']);
		Query::check_hash($assoc['hash'], $login.$pass);
		
		$pdo = Query::connection();	
		$request = $pdo->prepare("SELECT `id`, `fio`, `access`, `id_park` FROM `users` WHERE `login` = :login AND `password` = :pass");
		$request->execute(array(':login' => $login, ':pass' => $pass));
		$result = $request->fetch(PDO::FETCH_ASSOC);
		if(empty($result))
		{
			print_data(array(ERR_TITLE => Errors::LOGIN_OR_PASS,
							  STATIS => "-4"));
		}

		Query::print_response($result);
	}
	
	public static function SelectUsers($assoc)
	{
		$pdo = Query::connection();
		$request = $pdo->prepare("SELECT * FROM `users`");
		$request->execute();
		Query::print_array_response($request);
	}
	
	public static function InsertUser($assoc)
	{
		if(!isset($assoc['login']) ||
		   !isset($assoc['lpass']) ||
		   !isset($assoc['fio']) ||
		   !isset($assoc['access']) ||
		   !isset($assoc['id_park']) ||
		   !isset($assoc['hash']))
		{
			print_data(array(ERR_TITLE => Errors::REQUETS,
							  STATIS => "-1"));
		}
		$login = urldecode($assoc['login']);
		$pass = urldecode($assoc['lpass']);
		$fio = urldecode($assoc['fio']);
		Query::check_hash($assoc['hash'], $login.$pass.$fio.$assoc['access'].$assoc['id_park']);
		
		$pdo = Query::connection();
		$request = $pdo->prepare("INSERT INTO `users` (`login`, `password`, `fio`, `access`, `id_park`) VALUES (:login, :pass, :fio, :access, :id_park)");
		$values = array(':login' => $login, 
						':pass' => $pass, 
						':fio' => $fio, 
						':access' => $assoc['access'],
						':id_park' => $assoc['id_park']);
		$request->execute($values);
		$request = $pdo->prepare("SELECT `id` FROM `users` WHERE `login` = :login AND `password` = :pass AND `fio` = :fio AND `access` = :access AND `id_park` = :id_park");
		$request->execute($values);
		$result = $request->fetch(PDO::FETCH_ASSOC);
		if(empty($result))
		{
			print_data(array(ERR_TITLE => Errors::INSERT_USER,
							  STATIS => "-4"));
		}
		Query::print_response($result);
	}
	
	public static function UpdateUser($assoc)
	{
		if(!isset($assoc['id']) ||
		   !isset($assoc['login']) ||
		   !isset($assoc['lpass']) ||
		   !isset($assoc['fio']) ||
		   !isset($assoc['access']) ||
		   !isset($assoc['id_park']) ||
		   !isset($assoc['hash']))
		{
			print_data(array(ERR_TITLE => Errors::REQUETS,
							  STATIS => "-1"));
		}
		
		$login = urldecode($assoc['login']);
		$pass = urldecode($assoc['lpass']);
		$fio = urldecode($assoc['fio']);
		
		Query::check_hash($assoc['hash'], $assoc['id'].$login.$pass.$fio.$assoc['access'].$assoc['id_park']);
		
		$pdo = Query::connection();
		$request = $pdo->prepare("UPDATE `users` SET `login`=:login, `password`=:pass, `fio`=:fio, `access`=:access, `id_park`=:id_park WHERE `id`=:id");
		$values = array(':id' => $assoc['id'],
						':login' => $login, 
						':pass' => $pass, 
						':fio' => $fio, 
						':access' => $assoc['access'],
						':id_park' => $assoc['id_park']);
		$result = $request->execute($values);
		if($result == 0)
		{
			print_data(array(ERR_TITLE => Errors::UPDATE_USER,
							  STATIS => "-4"));
		}
		print_data(array(STATIS => "1"));
	}
	
	public static function RemoveUser($assoc)
	{
		if(!isset($assoc['id']) ||
		   !isset($assoc['hash']))
		{
			print_data(array(ERR_TITLE => Errors::REQUETS,
							  STATIS => "-1"));
		}
		
		Query::check_hash($assoc['hash'], $assoc['id']);
		
		$pdo = Query::connection();
		$request = $pdo->prepare("DELETE FROM `users` WHERE `id`=:id");
		$values = array(':id' => $assoc['id']);
		$result = $request->execute($values);
		if($result == 0)
		{
			print_data(array(ERR_TITLE => Errors::DELETE_USER,
							  STATIS => "-4"));
		}
		print_data(array(STATIS => "1"));
	}
//	end User	-----------------------------------------------------------------------------------------------------------

//	AutoPark	-----------------------------------------------------------------------------------------------------------
	public static function AutoParkNames($assoc)
	{				
		$pdo = Query::connection();
		$request = $pdo->prepare("SELECT `id`, `name` FROM `autoparks`");
		$request->execute();
		Query::print_array_response($request);
	}
	
	public static function AutoParkSetName($assoc)
	{
		if(!isset($assoc['id']) ||
		   !isset($assoc['park_name']) ||
		   !isset($assoc['hash']))
		{
			print_data(array(ERR_TITLE => Errors::REQUETS,
							  STATIS => "-1"));
		}
		$name = urldecode($assoc['park_name']);
		
		Query::check_hash($assoc['hash'], $assoc['id'].$name);
		
		$pdo = Query::connection();
		$request = $pdo->prepare("SELECT * FROM `autoparks` WHERE `id` = :id");
		$request->execute(array(':id' => $assoc['id']));
		$result = $request->fetch(PDO::FETCH_ASSOC);
		$values = array(':id' => $assoc['id'], 
						':name' => $name);
		if(empty($result))
			$request = $pdo->prepare("INSERT INTO `autoparks` (`id`, `name`) 
									  VALUES (:id, :name)");
		else
			$request = $pdo->prepare("UPDATE `autoparks` SET `name`=:name WHERE `id`=:id");
		$result = $request->execute($values);
		if($result == 0)
		{
			print_data(array(ERR_TITLE => Errors::AUTOPARK_SET_NAME,
							  STATIS => "-4"));
		}
		print_data(array(STATIS => "1"));
	}
//	end AutoPark	-------------------------------------------------------------------------------------------------------

//	Month		-----------------------------------------------------------------------------------------------------------
	public static function MonthSetDayValue($assoc)
	{
		if(!isset($assoc['id_driver']) ||
		   !isset($assoc['id_park']) ||
		   !isset($assoc['day_work']) ||
		   !isset($assoc['hours_enum']) ||
		   !isset($assoc['notify']) ||
		   !isset($assoc['attendance']) ||
		   !isset($assoc['node']) ||
		   !isset($assoc['id_driver_replaced']) ||
		   !isset($assoc['id_driver_replaceable']) ||
		   !isset($assoc['hash']))
		{
			print_data(array(ERR_TITLE => Errors::REQUETS,
							  STATIS => "-1"));
		}
		$node = urldecode($assoc['node']);
		$day_work = date('Y-m-d', $assoc['day_work']);
		
		Query::check_hash($assoc['hash'], $assoc['id_driver'].$assoc['id_park'].$assoc['day_work'].$assoc['hours_enum'].
										  $assoc['notify'].$assoc['attendance'].$node.$assoc['id_driver_replaced'].$assoc['id_driver_replaceable']);
		
		$autopark = 'autopark_'.$assoc['id_park'];
		$pdo = Query::connection();
		$request = $pdo->prepare("SELECT * FROM ".$autopark." WHERE `id_driver` = :id_driver AND `day_work` = :day_work");
		$request->execute(array(':id_driver' => $assoc['id_driver'], ':day_work' => $day_work));
		$result = $request->fetch(PDO::FETCH_ASSOC);
		$values = array(':id_driver' => $assoc['id_driver'], 
						':day_work' => $day_work,
						':hours_enum' => $assoc['hours_enum'],
						':notify' => $assoc['notify'],
						':attendance' => $assoc['attendance'],
						':id_driver_replaced' => $assoc['id_driver_replaced'],
						':id_driver_replaceable' => $assoc['id_driver_replaceable'],
						':node' => $node);
		if(empty($result))
			$request = $pdo->prepare("INSERT INTO ".$autopark." (`id_driver`, `day_work`, `hours_enum`, `notify`, `attendance`, `node`, `id_driver_replaced`, `id_driver_replaceable`) VALUES (:id_driver, :day_work, :hours_enum, :notify, :attendance, :node, :id_driver_replaced, :id_driver_replaceable)");
		else
			$request = $pdo->prepare("UPDATE ".$autopark." SET `hours_enum`=:hours_enum, `notify`=:notify, `attendance`=:attendance, `node`=:node, `id_driver_replaced`=:id_driver_replaced, `id_driver_replaceable`=:id_driver_replaceable WHERE `id_driver`=:id_driver AND `day_work` = :day_work");
		$result = $request->execute($values);
		if($result == 0)
		{
			print_data(array(ERR_TITLE => Errors::MONTH_SET_DAY_VALUE,
							  STATIS => "-4"));
		}
		print_data(array(STATIS => "1"));
	}
//	end Month	-----------------------------------------------------------------------------------------------------------

//	Driver		-----------------------------------------------------------------------------------------------------------
	public static function DriversSelectData($assoc)
	{
		if(!isset($assoc['id_park']) ||
		   !isset($assoc['hash']))
		{
			print_data(array(ERR_TITLE => Errors::REQUETS,
							  STATIS => "-1"));
		}
		
		Query::check_hash($assoc['hash'], $assoc['id_park']);
			
		$pdo = Query::connection();
		$sql = "SELECT `id`, `schedule`, `is_work`, `state`, `not_responding`, `time_to_sleep`, `sequence`, `comment`  FROM `drivers` WHERE `id_park`=:id_park";
		
		$request = $pdo->prepare($sql);
		$request->execute(array(':id_park' => $assoc['id_park']));
		Query::print_array_response($request);
	}
	
	public static function DriversSelectMonth($assoc)
	{
		if(!isset($assoc['id_park']) ||
		   !isset($assoc['hash']) ||
		   !isset($assoc['date']))
		{
			print_data(array(ERR_TITLE => Errors::REQUETS,
							  STATIS => "-1"));
		}
		
		$date = $assoc['date'];
		
		Query::check_hash($assoc['hash'], $assoc['id_park'].$date);
		
		$autopark = 'autopark_'.$assoc['id_park'];
		
		$date_start = date_create();
		date_timestamp_set($date_start, $date);
		date_modify($date_start, 'first day of this month');
		date_sub($date_start, date_interval_create_from_date_string('1 days'));
		$date_end = date_create();
		date_timestamp_set($date_end, $date);
		date_modify($date_end, 'last day of this month');
		date_add($date_end, date_interval_create_from_date_string('1 days'));
		 
		$pdo = Query::connection();
		$sql = "SELECT * FROM ".$autopark." WHERE (`day_work` BETWEEN CAST('".$date_start->format('Y-m-d')."' AS DATE) AND CAST('".$date_end->format('Y-m-d')."' AS DATE))";
		
		$request = $pdo->prepare($sql);
		$request->execute();
		Query::print_array_response($request);
	}
	
	public static function DriverSelectMonth($assoc)
	{
		if(!isset($assoc['id']) ||
		   !isset($assoc['id_park']) ||
		   !isset($assoc['hash']) ||
		   !isset($assoc['date']))
		{
			print_data(array(ERR_TITLE => Errors::REQUETS,
							  STATIS => "-1"));
		}
		
		$id = $assoc['id'];
		$date = $assoc['date'];
				
		Query::check_hash($assoc['hash'],$id.$date.$assoc['id_park']);
		
		$date_start = date_create();
		date_timestamp_set($date_start, $date);
		date_modify($date_start, 'first day of this month');
		date_sub($date_start, date_interval_create_from_date_string('1 days'));
		$date_end = date_create();
		date_timestamp_set($date_end, $date);
		date_modify($date_end, 'last day of this month');
		date_add($date_end, date_interval_create_from_date_string('1 days'));
		// 
		$autopark = 'autopark_'.$assoc['id_park'];
		
		$pdo = Query::connection();
		$sql = "SELECT * FROM ".$autopark." WHERE (`day_work` BETWEEN CAST('".$date_start->format('Y-m-d')."' AS DATE) AND CAST('".$date_end->format('Y-m-d')."' AS DATE)) AND `id_driver`=:id";
		
		$request = $pdo->prepare($sql);
		$request->execute(array(':id' => $id));
		Query::print_array_response($request);
	}
	
	//	Nodes
	public static function DriversSelectNodes($assoc)
	{
		if(!isset($assoc['id_park']) ||
		   !isset($assoc['hash']))
		{
			print_data(array(ERR_TITLE => Errors::REQUETS,
							  STATIS => "-1"));
		}
		
		$pdo = Query::connection();
		$sql = "SELECT `driver_nodes`.`id`, `driver_nodes`.`id_driver`, `driver_nodes`.`id_park`, `driver_nodes`.`id_user`, `driver_nodes`.`date_node`, `driver_nodes`.`node`, `users`.`fio` FROM `driver_nodes`, `users` WHERE `driver_nodes`.`id_park`=:id_park AND `users`.`id`=`driver_nodes`.`id_user`";
		
		$request = $pdo->prepare($sql);
		$request->execute(array(':id_park' => $assoc['id_park']));
		Query::print_array_response($request);
	}
	

	public static function DriverAddNode($assoc)
	{
		if(!isset($assoc['id_driver']) ||
		   !isset($assoc['id_park'])   ||
		   !isset($assoc['id_user']) ||
		   !isset($assoc['date_node']) ||
		   !isset($assoc['node']) ||
		   !isset($assoc['hash']))
		{
			print_data(array(ERR_TITLE => Errors::REQUETS,
							  STATIS => "-1"));
		}
		$node = urldecode($assoc['node']);
		
		Query::check_hash($assoc['hash'], $assoc['id_driver'].$assoc['id_park'].$assoc['id_user'].$assoc['date_node'].$node);
		
		$pdo = Query::connection();
		$values = array(':id_driver' => $assoc['id_driver'], 
						':id_park' => $assoc['id_park'],
						':id_user' => $assoc['id_user'],
						':date_node' => $assoc['date_node'],
						':node' => $node);
		$request = $pdo->prepare("INSERT INTO `driver_nodes` (`id_driver`, `id_park`, `id_user`, `date_node`, `node`) VALUES (:id_driver, :id_park, :id_user, :date_node, :node)");
		$result = $request->execute($values);
		if($result == 0)
		{
			print_data(array(ERR_TITLE => Errors::DRIVER_ADD_NODE,
							  STATIS => "-4"));
		}
		$request = $pdo->prepare("SELECT `id`, `date_node`, `node` FROM `driver_nodes` WHERE `id_driver` = :id_driver AND `id_park` = :id_park AND `id_user` = :id_user AND `date_node` = :date_node AND `node` = :node");
		$request->execute($values);
		$result = $request->fetch(PDO::FETCH_ASSOC);
		if(empty($result))
		{
			print_data(array(ERR_TITLE => Errors::DRIVER_ADD_NODE,
							  STATIS => "-4"));
		}
		Query::print_response($result);
	}
	
	public static function DriverEditNode($assoc)
	{
		if(!isset($assoc['id']) ||
		   !isset($assoc['node']) ||
		   !isset($assoc['hash']))
		{
			print_data(array(ERR_TITLE => Errors::REQUETS,
							  STATIS => "-1"));
		}
		
		$node = urldecode($assoc['node']);
		
		Query::check_hash($assoc['hash'], $assoc['id'].$node);
		
		$pdo = Query::connection();
		$sql = "UPDATE `driver_nodes` SET `node`=:node WHERE `id`=:id";
		
		$request = $pdo->prepare($sql);
		$result = $request->execute(array(':id' => $assoc['id'], ':node' => $node));
		if($result == 0)
		{
			print_data(array(ERR_TITLE => Errors::DRIVER_EDIT_NODE,
							  STATIS => "-4"));
		}
		print_data(array(STATIS => "1"));
	}
	
	public static function DriverRemoveNode($assoc)
	{
		if(!isset($assoc['id']) ||
		   !isset($assoc['hash']))
		{
			print_data(array(ERR_TITLE => Errors::REQUETS,
							  STATIS => "-1"));
		}
		
		Query::check_hash($assoc['hash'], $assoc['id']);
		
		$pdo = Query::connection();
		$request = $pdo->prepare("DELETE FROM `driver_nodes` WHERE `id`=:id");
		$values = array(':id' => $assoc['id']);
		$result = $request->execute($values);
		if($result == 0)
		{
			print_data(array(ERR_TITLE => Errors::DRIVER_REMOVE_NODE,
							  STATIS => "-4"));
		}
		print_data(array(STATIS => "1"));
	}
	//	end Nodes
	
	public static function DriverSetSchedule($assoc)
	{
		if(!isset($assoc['id']) ||
		   !isset($assoc['id_park']) ||
		   !isset($assoc['schedule']) ||
		   !isset($assoc['hash']))
		{
			print_data(array(ERR_TITLE => Errors::REQUETS,
							  STATIS => "-1"));
		}
		$schedule = urldecode($assoc['schedule']);
		
		Query::check_hash($assoc['hash'], $assoc['id'].$assoc['id_park'].$schedule);
		
		$pdo = Query::connection();
		$request = $pdo->prepare("SELECT * FROM `drivers` WHERE `id` = :id");
		$request->execute(array(':id' => $assoc['id']));
		$result = $request->fetch(PDO::FETCH_ASSOC);
		$values = array(':id' => $assoc['id'], 
						':id_park' => $assoc['id_park'], 
						':schedule' => $schedule);
		if(empty($result))
			$request = $pdo->prepare("INSERT INTO `drivers` (`id`, `id_park`, `schedule`) VALUES (:id, :id_park, :schedule)");
		else
			$request = $pdo->prepare("UPDATE `drivers` SET `schedule`=:schedule, `id_park`=:id_park WHERE `id`=:id");
		$result = $request->execute($values);
		if($result == 0)
		{
			print_data(array(ERR_TITLE => Errors::DRIVER_SET_SCHEDULE,
							  STATIS => "-4"));
		}
		print_data(array(STATIS => "1"));
	}
	
	public static function DriverSetNotWork($assoc)
	{
		if(!isset($assoc['id']) ||
		   !isset($assoc['hash']))
		{
			print_data(array(ERR_TITLE => Errors::REQUETS,
							  STATIS => "-1"));
		}
		
		Query::check_hash($assoc['hash'], $assoc['id']);
		
		$pdo = Query::connection();
		$request = $pdo->prepare("SELECT `id` FROM `drivers` WHERE `id` = :id");
		$values = array(':id' => $assoc['id']);
		$request->execute($values);
		$result = $request->fetch(PDO::FETCH_ASSOC);
		if(!empty($result))
		{
			$request = $pdo->prepare("UPDATE `drivers` SET `is_work`='0' WHERE `id`=:id");
			$result = $request->execute($values);
			if($result == 0)
			{
				print_data(array(ERR_TITLE => Errors::DRIVER_SET_WORK,
							  STATIS => "-4"));
			}
		}
		print_data(array(STATIS => "1"));
	}
	
	public static function DriverSetWork($assoc)
	{
		if(!isset($assoc['id_work']) ||
		   !isset($assoc['id_park']) ||
		   !is_array($assoc['ids']) ||
		   !isset($assoc['hash']))
		{
			print_data(array(ERR_TITLE => Errors::REQUETS,
							  STATIS => "-1"));
		}
		
		$ids = $assoc['ids'];
		
		$values = "";
		foreach($ids as $id)	$values .= $id;
		
		Query::check_hash($assoc['hash'], $assoc['id_work'].$assoc['id_park'].$values);
		
		$pdo = Query::connection();
		$request = $pdo->prepare("SELECT * FROM `drivers` WHERE `id` = :id");
		$values = array(':id' => $assoc['id_work'],
						':id_park' => $assoc['id_park']);
		$request->execute(array(':id' => $assoc['id_work']));
		$result = $request->fetch(PDO::FETCH_ASSOC);
		if(empty($result))
			$request = $pdo->prepare("INSERT INTO `drivers` (`id`, `id_park`, `is_work`) VALUES (:id, :id_park, '1')");
		else
			$request = $pdo->prepare("UPDATE `drivers` SET `is_work`='1', `id_park`=:id_park WHERE `id`=:id");
		$result = $request->execute($values);
		if($result == 0)
		{
			print_data(array(ERR_TITLE => Errors::DRIVER_SET_WORK,
							  STATIS => "-4"));
		}
		
		$sql = "UPDATE `drivers` SET `is_work`='0' WHERE";
		$values = array();
		$ids_count = count($ids);
		for($i = 0; $i < $ids_count; $i++)
		{
			$sql .= ' `id`=:id'.$i.' OR';
			$values[":id$i"] = $ids[$i];
		}
		if($ids_count == 0)
			print_data(array(STATIS => "1"));
			
		$sql = substr($sql, 0, strlen($sql)-3);
		$request = $pdo->prepare($sql);
		$request->execute($values);		
		
		print_data(array(STATIS => "1"));
	}
	
	public static function DriverSetState($assoc)
	{
		if(!isset($assoc['id']) ||
		   !isset($assoc['id_park']) ||
		   !isset($assoc['driver_state']) ||
		   !isset($assoc['hash']))
		{
			print_data(array(ERR_TITLE => Errors::REQUETS,
							  STATIS => "-1"));
		}
		
		Query::check_hash($assoc['hash'], $assoc['id'].$assoc['id_park'].$assoc['driver_state']);
		
		$pdo = Query::connection();
		$request = $pdo->prepare("SELECT * FROM `drivers` WHERE `id` = :id");
		$request->execute(array(':id' => $assoc['id']));
		$result = $request->fetch(PDO::FETCH_ASSOC);
		$values = array(':id' => $assoc['id'], 
						':id_park' => $assoc['id_park'], 
						':state' => $assoc['driver_state']);
		if(empty($result))
			$request = $pdo->prepare("INSERT INTO `drivers` (`id`, `id_park`, `state`) VALUES (:id, :id_park, :state)");
		else
			$request = $pdo->prepare("UPDATE `drivers` SET `state`=:state, `id_park`=:id_park WHERE `id`=:id");
		$result = $request->execute($values);
		if($result == 0)
		{
			print_data(array(ERR_TITLE => Errors::DRIVER_SET_STATE,
							  STATIS => "-4"));
		}
		
		if (!isset($assoc['month']) || !isset($assoc['year']))
			print_data(array(STATIS => "1"));
		else
			DeleteSubstitutionsDriver($assoc);
		print_data(array(STATIS => "1"));
	}
	
	public static function UpdateDriverNotResponding($assoc)
	{
		if(!isset($assoc['id']) ||
		   !isset($assoc['id_park']) ||
		   !isset($assoc['hash']))
		{
			print_data(array(ERR_TITLE => Errors::REQUETS,
							  STATIS => "-1"));
		}
		$id = $assoc['id'];
		$id_park = $assoc['id_park'];
		$NR = $assoc['not_responding'];
		
		Query::check_hash($assoc['hash'], $id.$id_park.$NR);
		$pdo = Query::connection();
		$request = $pdo->prepare("SELECT * FROM `drivers` WHERE `id` = :id");
		$request->execute(array(':id' => $id));
		$result = $request->fetch(PDO::FETCH_ASSOC);
		$values = array(':id' => $id, 
						':id_park' => $id_park, 
						':not_responding' => $NR);
						
		if(empty($result))
			$request = $pdo->prepare("INSERT INTO `drivers` (`id`, `id_park`, `not_responding`) VALUES (:id, :id_park, :not_responding)");
		else
			$request = $pdo->prepare("UPDATE drivers SET `not_responding` = :not_responding WHERE `id`=:id AND `id_park`=:id_park ");
			
		$result = $request->execute($values);
		if($result == 0)
		{
			print_data(array(ERR_TITLE => Errors::DRIVER_SET_STATE,
							  STATIS => "-4"));
		}
		print_data(array(STATIS => "1"));
	}
//	end Driver	-----------------------------------------------------------------------------------------------------------

//Drivers -----------------------------------------------------------------------------------------------------------------
	public static function CancellationReplacementDriver($assoc)
	{
		if(!isset($assoc['id']) ||
		   !isset($assoc['day_work']) ||
		   !isset($assoc['hash']))
		{
			print_data(array(ERR_TITLE => Errors::REQUETS,
							  STATIS => "-1"));
		}

		Query::check_hash($assoc['hash'], $assoc['id'].$assoc['day_work']);
		
		$autopark = 'autopark_'.$assoc['id_park'];
		
		$pdo = Query::connection();
		
		$values = array(':id' => $assoc['id'], 						
						':day_work' => $assoc['day_work']);
						
		$request_delete = $pdo->prepare("DELETE FROM ".$autopark." WHERE `id_driver_replaceable`=:id AND `day_work`=:day_work");
		$result_delete = $request_delete->execute($values);
		if($result_delete == 0)
		{
			print_data(array(ERR_TITLE => Errors::MONTH_CANCELLATION_REPLACEMENT_DRIVER,
							  STATIS => "-4"));
		}
		
		$request_update = $pdo->prepare("UPDATE ".$autopark." SET id_driver_replaced=0 WHERE id_driver=:id AND day_work=:day_work");
		$result_update = $request_update->execute($values);
		if($result_update == 0)
		{
			print_data(array(ERR_TITLE => Errors::MONTH_CANCELLATION_REPLACEMENT_DRIVER,
							  STATIS => "-4"));
		}
		
		
		print_data(array(STATIS => "1"));
	}
	
	public static function TimeToSleep($assoc)
	{
		if(!isset($assoc['id']) ||
		   !isset($assoc['id_park']) ||
		   !isset($assoc['timeSleep']) ||
		   !isset($assoc['hash']))
		{
			print_data(array(ERR_TITLE => Errors::REQUETS,
							  STATIS => "-1"));
		}
		
		Query::check_hash($assoc['hash'], $assoc['id'].$assoc['id_park'].$assoc['timeSleep']);
		$pdo = Query::connection();
		$request = $pdo->prepare("SELECT * FROM `drivers` WHERE `id` = :id");
		$request->execute(array(':id' => $assoc['id']));
		$result = $request->fetch(PDO::FETCH_ASSOC);
		
		$values = array(':id' => $assoc['id'], 
						':id_park' => $assoc['id_park'],
						':timeToSleep' => $assoc['timeSleep']);
		if(empty($result))
			$request = $pdo->prepare("INSERT INTO `drivers` (`id`, `id_park`, `time_to_sleep`) VALUES (:id, :id_park, :timeToSleep)");
		else
			$request = $pdo->prepare("UPDATE `drivers` SET `time_to_sleep`=:timeToSleep WHERE `id`=:id AND `id_park`=:id_park ");
			
		$result = $request->execute($values);
		if($result == 0)
		{
			print_data(array(ERR_TITLE => Errors::DRIVER_TIME_TO_SLEEP,
							  STATIS => "-4"));
		}
		print_data(array(STATIS => "1"));
	}
	
		public static function DriverIntercession($assoc)
	{
		if(!isset($assoc['id']) ||
		   !isset($assoc['id_park']) ||
		   !isset($assoc['comment']) ||
		   !isset($assoc['hash']))
		{
			print_data(array(ERR_TITLE => Errors::REQUETS,
							  STATIS => "-1"));
		}
		
		Query::check_hash($assoc['hash'], $assoc['id'].$assoc['id_park'].$assoc['comment']);
		$pdo = Query::connection();
		$request = $pdo->prepare("SELECT * FROM `drivers` WHERE `id` = :id");
		$request->execute(array(':id' => $assoc['id']));
		$result = $request->fetch(PDO::FETCH_ASSOC);
		
		$values = array(':id' => $assoc['id'], 
						':id_park' => $assoc['id_park'],
						':comment' => $assoc['comment']);
		if(empty($result))
			$request = $pdo->prepare("INSERT INTO `drivers` (`id`, `id_park`, `comment`) VALUES (:id, :id_park, :comment)");
		else
			$request = $pdo->prepare("UPDATE `drivers` SET `comment`=:comment WHERE `id`=:id AND `id_park`=:id_park ");
			
		$result = $request->execute($values);
		if($result == 0)
		{
			print_data(array(ERR_TITLE => Errors::DRIVER_INRERCESSION,
							  STATIS => "-4"));
		}
		print_data(array(STATIS => "1"));
	}
// end Drivers ------------------------------------------------------------------------------------------------------------

// Month ------------------------------------------------------------------------------------------------------------------
	public static function autoDayMonth($assoc)
	{
		if(!isset($assoc['id']) ||
		   !isset($assoc['id_park']) ||
		   !isset($assoc['year']) ||
		   !isset($assoc['month']) ||
		   !isset($assoc['schedule']) ||
		   !isset($assoc['hash']))
		{
			print_data(array(ERR_TITLE => Errors::REQUETS,
							  STATIS => "-1"));
		}
		
		$id = $assoc['id'];
		$schedule = $assoc['schedule'];
		$call_day = cal_days_in_month(CAL_GREGORIAN, (int)$assoc['month'], (int)$assoc['year']);
		$start = 1;
		$end = $start+$schedule-1;
		$dataStart = $assoc['year']."-".$assoc['month']."-".(string)$start;
		$dataEnd = $assoc['year']."-".$assoc['month']."-".(string)$end;
		
		Query::check_hash($assoc['hash'], $assoc['id'].$assoc['id_park'].$assoc['year'].$assoc['month'].$assoc['schedule']);
		
		$autopark = 'autopark_'.$assoc['id_park'];
		
		$pdo = Query::connection();
		$values = array(':id' => $id,
						':dataStart' => $dataStart,
						':dataEnd' => $dataEnd
						);
		$request = $pdo->prepare("SELECT hours_enum FROM ".$autopark." WHERE id_driver = :id AND day_work BETWEEN :dataStart AND :dataEnd ORDER BY day_work");
		$graphics_array = $request->execute($values);							
		if($graphics_array == 0)
		{
			print_data(array(ERR_TITLE => Errors::AUTOMATIC_SCHEDULE_DRIVER,
							  STATIS => "-4"));
		}
		$graphics_array = $request->fetchAll(PDO::FETCH_COLUMN, 0);
		
		if (count($graphics_array) != $schedule)
		{
			print_data(array(ERR_TITLE => "��������� ������ �� ������ ".$schedule." ���",
							  STATIS => "-4"));
		}
		
		$sch = 0;

		for ($i=$schedule+1; $i <= $call_day; $i++)
		{
			$newData = $assoc['year']."-".$assoc['month']."-".(string)$i;
			$values = array(
						':id' => $id,
						':day_work' => $newData,
						':hours_enum' => $graphics_array[$sch]
						);
			$request = $pdo->prepare("SELECT * FROM ".$autopark." WHERE `id_driver` = :id AND `day_work` = :day_work");
			$request->execute(array(':id' => $id, ':day_work' => $newData));
			$result = $request->fetch(PDO::FETCH_ASSOC);
			if(empty($result))
				$request = $pdo->prepare("INSERT INTO ".$autopark." (`id_driver`, `day_work`,`hours_enum`) VALUES (:id, :day_work, :hours_enum)");
			else
				$request = $pdo->prepare("UPDATE ".$autopark." SET `hours_enum` = :hours_enum WHERE `id_driver` = :id AND `day_work` = :day_work");
			$result = $request->execute($values);							
			if($result == 0)
			{
				print_data(array(ERR_TITLE => Errors::AUTOMATIC_SCHEDULE_DRIVER,
								  STATIS => "-4"));
			}
			
			++$sch;
			if ($sch >= $schedule)
				$sch = 0;
		}	
		print_data(array(STATIS => "1"));		
	}
	
	public static function autoDayNextMonth($assoc)
	{
		if(!isset($assoc['id']) ||
		   !isset($assoc['id_park']) ||
		   !isset($assoc['year']) ||
		   !isset($assoc['month']) ||
		   !isset($assoc['newYear']) ||
		   !isset($assoc['newMonth']) ||
		   !isset($assoc['schedule']) ||
		   !isset($assoc['hash']))
		{
			print_data(array("error_msg" => Errors::REQUETS,
							  STATIS => "-1"));
		}
		
		$id = $assoc['id'];
		$schedule = $assoc['schedule'];
		$old_call_day = cal_days_in_month(CAL_GREGORIAN, (int)$assoc['month'], (int)$assoc['year']);
		$call_day = cal_days_in_month(CAL_GREGORIAN, (int)$assoc['newMonth'], (int)$assoc['newYear']);
		$start = $old_call_day - $schedule + 1;
		$end = $old_call_day;
		$dataStart = $assoc['year']."-".$assoc['month']."-".(string)$start;
		$dataEnd = $assoc['year']."-".$assoc['month']."-".(string)$end;
		
		Query::check_hash($assoc['hash'], $assoc['id'].$assoc['id_park'].$assoc['year'].$assoc['month'].$assoc['newYear'].$assoc['newMonth'].$assoc['schedule']);
		
		$autopark = 'autopark_'.$assoc['id_park'];
		
		$pdo = Query::connection();
		$values = array(':id' => $id,
						':dataStart' => $dataStart,
						':dataEnd' => $dataEnd
						);
		$request = $pdo->prepare("SELECT hours_enum FROM ".$autopark." WHERE id_driver = :id AND day_work BETWEEN :dataStart AND :dataEnd ORDER BY day_work");
		$graphics_array = $request->execute($values);							
		if($graphics_array == 0)
		{
			print_data(array("error_msg" => "ERROR SELECT",
							  "status" => "-4"));
		}
		$graphics_array = $request->fetchAll(PDO::FETCH_COLUMN, 0);
		
		if (count($graphics_array) != $schedule)
		{
		//	print_data(array(ERR_TITLE => "��������� ������ ��������� ".$schedule." ���",
		//					  STATIS => "-4"));
			print_data(array("error_msg" => "COUNT SELECT".count($graphics_array)."  ".$schedule,
							  "status" => "-4"));
		}
		
		$sch = 0;

		for ($i = 1; $i <= $call_day; $i++)
		{
			$newData = $assoc['newYear']."-".$assoc['newMonth']."-".(string)$i;
			$values = array(
						':id' => $id,
						':day_work' => $newData,
						':hours_enum' => $graphics_array[$sch]
						);
			$request = $pdo->prepare("SELECT * FROM ".$autopark." WHERE `id_driver` = :id AND `day_work` = :day_work");
			$request->execute(array(':id' => $id, ':day_work' => $newData));
			$result = $request->fetch(PDO::FETCH_ASSOC);
			if(empty($result))
				$request = $pdo->prepare("INSERT INTO ".$autopark." (`id_driver`, `day_work`, `hours_enum`) VALUES (:id, :day_work, :hours_enum)");
			else
				$request = $pdo->prepare("UPDATE ".$autopark." SET `hours_enum` = :hours_enum WHERE `id_driver` = :id AND `day_work` = :day_work");
			$result = $request->execute($values);							
			if($result == 0)
			{
				print_data(array("error_msg" => "ERROR INSERT",
								  "status" => "-4"));
			}
			
			++$sch;
			if ($sch >= $schedule)
				$sch = 0;
		}	
		print_data(array(STATIS => "1"));		
	}
// end Month --------------------------------------------------------------------------------------------------------------

// spoofing ---------------------------------------------------------------------------------------------------------------
	public static function SelectSubstitution($assoc)
	{
		if(!isset($assoc['id_park']))
		{
			print_data(array(ERR_TITLE => Errors::REQUETS, STATIS => "-1"));
		}
		
		$pdo = Query::connection();
		$request = $pdo->prepare("SELECT * FROM `spoofing` WHERE `id_park` = :id_park");
		$request->execute(array('id_park' => $assoc['id_park']));
		Query::print_array_response($request);	
	}
	
	public static function UpdateSubstitution($assoc)
	{
		if(!isset($assoc['id_old']) || !isset($assoc['id_park']) || !isset($assoc['id_new']) || !isset($assoc['year']) || !isset($assoc['month']) || !isset($assoc['hash']))
		{
			print_data(array(ERR_TITLE => Errors::REQUETS, STATIS => "-1"));
		}
		
		Query::check_hash($assoc['hash'], $assoc['id_old'].$assoc['id_park'].$assoc['id_new'].$assoc['year'].$assoc['month']);
		
		$autopark = 'autopark_'.$assoc['id_park'];		
		
		$pdo = Query::connection();
		
		$DateLast = cal_days_in_month(CAL_GREGORIAN, (int)$assoc['month'], (int)$assoc['year']);
		$DateFirst = 1;
		$dateStart = $assoc['year']."-".$assoc['month']."-".(string)$DateFirst;
		$dateEnd = $assoc['year']."-".$assoc['month']."-".(string)$DateLast;
		
		$request = $pdo->prepare("UPDATE ".$autopark." SET id_driver = :id_new WHERE id_driver = :id_old AND (day_work BETWEEN :DateStart AND :DateEnd);");
		$result = $request->execute(array(':id_new' => $assoc['id_new'], ':id_old' => $assoc['id_old'], ':DateStart' => $dateStart, ':DateEnd' => $dateEnd));							
		if(empty($result))
		{
			print_data(array(ERR_TITLE => Errors::ERROR_UPDATE_RECORD_SUBSTITUTION_MONTHS, STATIS => "-4"));
		}
		
		$request = $pdo->prepare("DELETE FROM spoofing WHERE `id` = :id_old AND id_park = :id_park");
		$result = $request->execute(array(':id_old' => $assoc['id_old'], 'id_park' => $assoc['id_park']));							
		if(empty($result))
		{
			print_data(array(ERR_TITLE => Errors::ERROR_DELETE_RECORD_SUBSTITUTION, STATIS => "-4"));
		}
		
		/*
		$request = $pdo->prepare("UPDATE drivers SET sequence = (SELECT sequence FROM (select * from drivers) as driverss WHERE id = :id_old) WHERE id = :id_new AND id_park = :id_park");
		$result = $request->execute(array(':id_old' => $assoc['id_old'], ':id_new' => $assoc['id_new'], ':id_park' => $assoc['id_park']));							
		if(empty($result))
		{
			print_data(array(ERR_TITLE => Errors::ERROR_UPDATE_RECORD_SUBSTITUTION, STATIS => "-4"));
		}
		*/
		
		print_data(array(STATIS => "1"));	
	}
	
	public static function InsertSubstitution($assoc)
	{
		if(!isset($assoc['id_park']) || !isset($assoc['id_auto']) || !isset($assoc['hash']))
		{
			print_data(array(ERR_TITLE => Errors::REQUETS, STATIS => "-1"));
		}
			
		$pdo = Query::connection();
		$request = $pdo->prepare("INSERT INTO spoofing (id_auto, id_park) VALUES(:id_auto, :id_park)");
		$result = $request->execute(array(':id_auto' => $assoc['id_auto'], ':id_park' => $assoc['id_park']));							
		if(empty($result))
		{
			print_data(array(ERR_TITLE => Errors::ERROR_ADD_RECORD_SUBSTITUTION, STATIS => "-4"));
		}
			
		$request = $pdo->prepare("SELECT * FROM spoofing WHERE id = (SELECT LAST_INSERT_ID())");
		$request->execute();
		
		Query::print_array_response($request);		
	}
	
	public static function DeleteSubstitution($assoc)
	{
		if(!isset($assoc['id_driver']) || !isset($assoc['id_park']) || !isset($assoc['hash']))
		{
			print_data(array(ERR_TITLE => Errors::REQUETS, STATIS => "-1"));
		}
		
		Query::check_hash($assoc['hash'], $assoc['id_driver'].$assoc['id_park']);
		
		$pdo = Query::connection();
		$request = $pdo->prepare("DELETE FROM spoofing WHERE `id` = :id_driver AND id_park = :id_park");
		$result = $request->execute(array(':id_driver' => $assoc['id_driver'], 'id_park' => $assoc['id_park']));							
		if(empty($result))
		{
			print_data(array(ERR_TITLE => Errors::ERROR_DELETE_RECORD_SUBSTITUTION, STATIS => "-4"));
		}
		
		$id = $assoc['id_driver'];
		$park = $assoc['id_park'];
		$request = $pdo->prepare("DELETE FROM  drivers WHERE `id` = ".$id." AND id_park = ".$park);
		$result = $request->execute();							
		if(empty($result))
		{
			print_data(array(ERR_TITLE => Errors::ERROR_DELETE_RECORD_SUBSTITUTION, STATIS => "-4"));
		}
		
		print_data(array(STATIS => "1"));		
	}
	
	public static function DeleteSubstitutionsDriver($assoc)
	{
		if(!isset($assoc['id_driver']) || 
		!isset($assoc['id_park']) || 
		!isset($assoc['month']) || 
		!isset($assoc['year']) ||
		!isset($assoc['hash']))
		{
			print_data(array(ERR_TITLE => Errors::REQUETS, STATIS => "-1"));
		}
	
		Query::check_hash($assoc['hash'], $assoc['id_driver'].$assoc['id_park'].$assoc['month'].$assoc['year']);
		
		$start = 1;		
		$end = cal_days_in_month(CAL_GREGORIAN, (int)$assoc['month'], (int)$assoc['year']);
		$dataStart = $assoc['year']."-".$assoc['month']."-".(string)$start;
		$dataEnd = $assoc['year']."-".$assoc['month']."-".(string)$end;
		$autopark = 'autopark_'.$assoc['id_park'];
		
		//$file = 'c:/Progect QT/test.txt';
		//file_put_contents($file, "");
        //	$current = file_get_contents($file);
        //	file_put_contents($file, $current."DeleteSubstitutionsDriver error 2"."\n");
		
		$pdo = Query::connection();
		
		$values = array(
						':id' => $assoc['id_driver'],
					    ':dataStart' => $dataStart,
					    ':dataEnd' => $dataEnd
					   );

		$request = $pdo->prepare("SELECT day_work, id_driver_replaced FROM ".$autopark." WHERE `id_driver`=:id  AND id_driver_replaced <> 0 AND `day_work` BETWEEN :dataStart AND :dataEnd");
		$request->execute($values);
		$array = $request->fetchAll();
				
		//if(empty($array))
		//{
		//	print_data(array("test 2", STATIS => "-4"));
		//}	
			
		for ($i = 0; $i < count($array); $i++)
		{
			$elements = $array[$i];
			$sqlQuery = "UPDATE ".$autopark." SET id_driver_replaceable = 0 WHERE id_driver = ".$elements["id_driver_replaced"]." AND day_work = '".$elements["day_work"]."'";
			$request = $pdo->prepare($sqlQuery);
			$result = $request->execute();	
			if(empty($result))
			{
				print_data(array("test 3", STATIS => "-4"));
			}
		}
		
		$request = $pdo->prepare("UPDATE ".$autopark." SET `id_driver_replaced` = 0 AND `id_driver_replaceable` = 0 WHERE `id_driver`=:id AND `day_work` BETWEEN :dataStart AND :dataEnd");
		$result = $request->execute($values);	
		if(empty($result))
		{
			print_data(array("test 4", STATIS => "-4"));
		}
		
		
		print_data(array(STATIS => "1"));		
	}
// end spoofing -----------------------------------------------------------------------------------------------------------

// sequence drivers on cars -----------------------------------------------------------------------------------------------
	public static function InsertSequenceDrivers($assoc)
	{
		if(!isset($assoc['id']) ||
		   !isset($assoc['id_park']) ||
		   !isset($assoc['sequence']) ||
		   !isset($assoc['hash']))
		{
			print_data(array(ERR_TITLE => Errors::REQUETS,
							  STATIS => "-1"));
		}
		
		Query::check_hash($assoc['hash'], $assoc['id'].$assoc['id_park'].$assoc['sequence']);	
			
		//$file = 'c:/Progect QT/test.txt';
		//file_put_contents($file, "");
        //$current = file_get_contents($file);
        //file_put_contents($file, $current.$sqlQuery."\n");

		$pdo = Query::connection();
		$request = $pdo->prepare("SELECT * FROM drivers WHERE id = :id AND id_park = :id_park");
		$request->execute(array(':id' => $assoc['id'], ':id_park' => $assoc['id_park']));
		$result = $request->fetch(PDO::FETCH_ASSOC);
		if(empty($result))
			$sqlQuery = "INSERT INTO `drivers` (`id`, `id_park`, `sequence`) VALUES (".$assoc['id'].",".$assoc['id_park'].",".$assoc['sequence'].")";
		else
			$sqlQuery = "UPDATE drivers SET `sequence` = ".$assoc['sequence']." WHERE `id` = ".$assoc['id']." AND `id_park` = ".$assoc['id_park'];

			
		$request = $pdo->prepare($sqlQuery);
		$result = $request->execute();
		if($result == 0)
		{
			print_data(array(ERR_TITLE => Errors::ERROR_UPDATE_SEQUENCE_DRIVERS_ON_CARS, STATIS => "-4"));
		}
		print_data(array(STATIS => "1"));
	}
	
	public static function ClearSequenceDrivers($assoc)
	{
		if(!isset($assoc['id']) ||
		   !isset($assoc['id_park']) ||
		   !isset($assoc['hash']))
		{
			print_data(array(ERR_TITLE => Errors::REQUETS, STATIS => "-1"));
		}
		
		Query::check_hash($assoc['hash'], $assoc['id'].$assoc['id_park'].$assoc['timeSleep']);
		$pdo = Query::connection();
		$request = $pdo->prepare("UPDATE `drivers` SET `sequence` = 0 WHERE `id`= ".$assoc['id']." AND `id_park` = ".$assoc['id_park']);
		$result = $request->execute();
		if($result == 0)
		{
			print_data(array(ERR_TITLE => Errors::ERROR_UPDATE_SEQUENCE_DRIVERS_ON_CARS, STATIS => "-4"));
		}
		print_data(array(STATIS => "1"));
	}
// end sequence drivers on cars -------------------------------------------------------------------------------------------

// start create table autoparks--------------------------------------------------------------------------------------------
	public static function CreateTablesAutoparks($assoc)
	{
		if(!isset($assoc['kol']) ||
		   !isset($assoc['hash']))
		{
			print_data(array(ERR_TITLE => Errors::REQUETS,
							  STATIS => "-1"));
		}
		
		Query::check_hash($assoc['hash'], $assoc['kol']);
		$pdo = Query::connection();
		for ($i = 0; $i < $assoc['kol']; $i++)
		{
			$str = (string)$i;
			$autopark = "autopark_".$str;
			$request = $pdo->prepare("CREATE TABLE IF NOT EXISTS ".$autopark." (`id_driver` BIGINT(20) UNSIGNED NOT NULL DEFAULT '0', `day_work` DATE NOT NULL DEFAULT '1970-01-01', `hours_enum` TINYINT(3) UNSIGNED NOT NULL DEFAULT '0',	`attendance` TINYINT(3) UNSIGNED NOT NULL DEFAULT '0', `notify` TINYINT(3) UNSIGNED NOT NULL DEFAULT '0', `node` VARCHAR(150) NOT NULL DEFAULT '' COLLATE 'utf8_unicode_ci', `id_driver_replaced` BIGINT(20) UNSIGNED NOT NULL DEFAULT '0', `id_driver_replaceable` BIGINT(20) UNSIGNED NOT NULL DEFAULT '0', INDEX `id_driver` (`id_driver`)) COLLATE='utf8_unicode_ci' ENGINE=InnoDB;");
			$result = $request->execute();
			
			if($result == 0)
				{
					print_data(array(ERR_TITLE => Errors::AUTOMATIC_SCHEDULE_DRIVER,
									  STATIS => "-4"));
				}
		}
		print_data(array(STATIS => "1"));
	}
// end create table autoparks----------------------------------------------------------------------------------------------

// Current version --------------------------------------------------------------------------------------------------------

	public static function CurrentVersion($assoc)
	{
		if(!isset($assoc['version']) ||
		   !isset($assoc['hash']))
		{
			print_data(array(ERR_TITLE => Errors::REQUETS,
							  STATIS => "-1"));
		}
		
		Query::check_hash($assoc['hash'], $assoc['version']);
		$pdo = Query::connection();
		$request = $pdo->prepare("SELECT * FROM version");
		$request->execute();
        Query::print_array_response($request);
	}
// end Current version ----------------------------------------------------------------------------------------------------

    public static function SaveRadialStatusCommand($assoc)
    {
        if(!isset($assoc['id']) || !isset($assoc['status']) || !isset($assoc['id_autopark']))
        {
            print_data(array(ERR_TITLE => "SaveRadialStatusCommand error",
                STATIS => "-1"));
        }

        $pdo = Query::connection();
        $request = $pdo->prepare("UPDATE "."autopark_".$assoc["id_autopark"]." SET `status_radial`=".$assoc["status"]." WHERE `id`=".$assoc['id']);
        $result = $request->execute();

        if(!$result)
        {
            print_data(array(ERR_TITLE => Errors::ERROR_UPDATE_RECORD_SUBSTITUTION,
                STATIS => "-4"));
        }


        print_data(array(STATIS => "1"));
    }
	
	
    public static function StatredProgramCommand($assoc)
    {
		if(!isset($assoc['key']) ||
		   !isset($assoc['hash']))
		{
			print_data(array(ERR_TITLE => Errors::REQUETS,
							  STATIS => "-1"));
		}
		
		Query::check_hash($assoc['hash'], $assoc['key']);

		$pdo = Query::connection();
		$request = $pdo->prepare("SELECT started_program FROM version");
		$request->execute();
        Query::print_array_response($request);
    }
	
	    public static function SetStatredProgramCommand($assoc)
    {
		if(!isset($assoc['flag']) ||
		   !isset($assoc['hash']))
		{
			print_data(array(ERR_TITLE => Errors::REQUETS,
							  STATIS => "-1"));
		}
		
		Query::check_hash($assoc['hash'], $assoc['flag']);
		
		$sql = null;
		if ($assoc['flag'] == 'true')
			$sql = "UPDATE version SET started_program=1";
		else if ($assoc['flag'] == 'false')
			$sql = "UPDATE version SET started_program=0";
		
		$pdo = Query::connection();
		$request = $pdo->prepare($sql);
		$result = $request->execute();

        if(!$result)
        {
            print_data(array("ERROR UPDATE started_program",
                STATIS => "-4"));
        }
		
        print_data(array(STATIS => "1"));
    }
}
?>