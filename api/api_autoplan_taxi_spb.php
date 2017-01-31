<?php
require_once 'crypt.php';
require_once 'config.php';
require_once 'errors.php';
require_once 'constants.php';

function print_data($error)
{
	print(json_encode($error, JSON_UNESCAPED_UNICODE | JSON_UNESCAPED_SLASHES));
	exit();
}
require_once 'query.php';

header('Content-type: application/json');

//$file_post = 'c:/Progect QT/post.txt';

if($_SERVER["REQUEST_METHOD"] != 'POST' || 	!isset($_POST['*']) || 	empty($_POST['*']))
{
    if ($_SERVER["REQUEST_METHOD"] == 'POST' || $_POST[TYPE_QUERY] == Query::TYPE_TEST)
    {
		//file_put_contents($file_post, $_POST);
        $assoc = $_POST;
    }
    else
    {
        print_data(array(ERR_TITLE => /*Errors::REQUETS*/
            'REQUEST_METHOD', STATIS => "-1"));
    }
}
else
{
    parse_str(decrypt($_POST['*']), $assoc);
}

if(!isset($assoc[NAME]) || !isset($assoc[PASS]) ||
   $assoc[NAME] != Config::LOGIN_HOST || 
   $assoc[PASS] != Config::PASS_HOST)
{
	print_data(array(ERR_TITLE => Errors::HOST_LOGIN_OR_PASS,
					  STATIS => "-2"));
}

if(!isset($assoc[TYPE_QUERY]) || empty($assoc[TYPE_QUERY]))
{
	print_data(array(ERR_TITLE => /*Errors::REQUETS,*/
                "TYPE_QUERY",
					  STATIS => "-1"));
}

//date_default_timezone_set('Europe/Moscow');

if($assoc[TYPE_QUERY] == Query::TYPE_LOGIN)
	Query::Login($assoc);
elseif($assoc[TYPE_QUERY] == Query::TYPE_SELECT_USERS)
	Query::SelectUsers($assoc);
elseif($assoc[TYPE_QUERY] == Query::TYPE_INSERT_USER)
	Query::InsertUser($assoc);
elseif($assoc[TYPE_QUERY] == Query::TYPE_UPDATE_USER)
	Query::UpdateUser($assoc);
elseif($assoc[TYPE_QUERY] == Query::TYPE_DELETE_USER)
	Query::RemoveUser($assoc);
elseif($assoc[TYPE_QUERY] == Query::TYPE_AUTOPARK_NAMES)
	Query::AutoParkNames($assoc);
elseif($assoc[TYPE_QUERY] == Query::TYPE_AUTOPARK_SET_NAME)
	Query::AutoParkSetName($assoc);
elseif($assoc[TYPE_QUERY] == Query::TYPE_MONTH_SET_DAY_VALUE)
	Query::MonthSetDayValue($assoc);
elseif($assoc[TYPE_QUERY] == Query::TYPE_DRIVERS_SELECT_DATA)
	Query::DriversSelectData($assoc);
elseif($assoc[TYPE_QUERY] == Query::TYPE_DRIVERS_SELECT_MONTH)
	Query::DriversSelectMonth($assoc);
elseif($assoc[TYPE_QUERY] == Query::TYPE_DRIVER_SELECT_MONTH)
	Query::DriverSelectMonth($assoc);
elseif($assoc[TYPE_QUERY] == Query::TYPE_DRIVERS_SELECT_NODES)
	Query::DriversSelectNodes($assoc);
elseif($assoc[TYPE_QUERY] == Query::TYPE_DRIVER_SET_SHEDULE)
	Query::DriverSetSchedule($assoc);
elseif($assoc[TYPE_QUERY] == Query::TYPE_DRIVER_SET_WORK)
	Query::DriverSetWork($assoc);
elseif($assoc[TYPE_QUERY] == Query::TYPE_DRIVER_SET_NOT_WORK)
	Query::DriverSetNotWork($assoc);
elseif($assoc[TYPE_QUERY] == Query::TYPE_DRIVER_ADD_NODE)
	Query::DriverAddNode($assoc);
elseif($assoc[TYPE_QUERY] == Query::TYPE_DRIVER_EDIT_NODE)
	Query::DriverEditNode($assoc);
elseif($assoc[TYPE_QUERY] == Query::TYPE_DRIVER_REMOVE_NODE)
	Query::DriverRemoveNode($assoc);
elseif($assoc[TYPE_QUERY] == Query::TYPE_DRIVER_SET_STATE)
	Query::DriverSetState($assoc);
//elseif($assoc[TYPE_QUERY] == Query::TYPE_CAR_SET_TIME_CHANGE_DRIVER)
//	Query::CarSetTimeChangeDriver($assoc);
elseif($assoc[TYPE_QUERY] == Query::TYPE_DRIVER_NOT_RESPONDING)
	Query::UpdateDriverNotResponding($assoc);
elseif($assoc[TYPE_QUERY] == Query::TYPE_CANCELLATION_REPLACEMENT_DRIVER)
	Query::CancellationReplacementDriver($assoc);
elseif($assoc[TYPE_QUERY] == Query::TYPE_TIME_TO_SLEEP)
	Query::TimeToSleep($assoc);	
elseif($assoc[TYPE_QUERY] == Query::TYPE_AUTO_DAY_MONTH)
	Query::autoDayMonth($assoc);	
elseif($assoc[TYPE_QUERY] == Query::TYPE_AUTO_NEXT_MONTH)
	Query::autoDayNextMonth($assoc);	
elseif($assoc[TYPE_QUERY] == Query::TYPE_INSERT_SUBSTITUTION)
	Query::InsertSubstitution($assoc);
elseif($assoc[TYPE_QUERY] == Query::TYPE_UPDATE_SUBSTITUTION)
	Query::UpdateSubstitution($assoc);
elseif($assoc[TYPE_QUERY] == Query::TYPE_SELECT_SUBSTITUTION)
	Query::SelectSubstitution($assoc);
elseif($assoc[TYPE_QUERY] == Query::TYPE_DELETE_SUBSTITUTION)
	Query::DeleteSubstitution($assoc);
elseif($assoc[TYPE_QUERY] == Query::TYPE_CREATE_TABLES_AUTOPARKS)
	Query::CreateTablesAutoparks($assoc);
elseif($assoc[TYPE_QUERY] == Query::TYPE_CURRENT_VERSION)
	Query::CurrentVersion($assoc);
elseif($assoc[TYPE_QUERY] == Query::TYPE_INSERT_SEQUENCE_DRIVERS)
	Query::InsertSequenceDrivers($assoc);
elseif($assoc[TYPE_QUERY] == Query::TYPE_CLEAR_SEQUENCE_DRIVERS)
	Query::ClearSequenceDrivers($assoc);
elseif($assoc[TYPE_QUERY] == Query::TYPE_SUBSTITUTIONS_DRIVER)
	Query::DeleteSubstitutionsDriver($assoc);
elseif($assoc[TYPE_QUERY] == Query::TYPE_DRIVER_INTERACESSION)
	Query::DriverIntercession($assoc);
elseif($assoc[TYPE_QUERY] == Query::TYPE_SAVE_RADIAL_STATUS)
    Query::SaveRadialStatusCommand($assoc);
elseif($assoc[TYPE_QUERY] == Query::TYPE_STARTED_RECALL_PROGRAM)
    Query::StatredProgramCommand($assoc);
elseif($assoc[TYPE_QUERY] == Query::TYPE_SET_STARTED_RECALL_PROGRAM)
    Query::SetStatredProgramCommand($assoc);
	
print_data(array(ERR_TITLE => Errors::REQUETS,
				 STATIS => "-1"));

?>