<?php
class Errors
{
	const REQUETS = 'Неправильно сформирован запрос.';
	const HASH = 'Неправильно вычислен хэш.';
	const HOST_LOGIN_OR_PASS = 'Неверный передан логин или/и пароль для авторизации на api сервере.';
	const LOGIN_OR_PASS = "Введен неверный логин или/и пароль.";
	const INSERT_USER = "Во время добавления нового пользователя произошла ошибка.";
	const UPDATE_USER = "Во время измения данных пользователя произошла ошибка.";
	const DELETE_USER = "Во время удаления данных пользователя произошла ошибка.";
	const CONNECTION_DB = 'Ошибка соединения к базе данных.';
	const AUTOPARK_SET_NAME = 'Не удалось изменить имя автопарка.';
	const CAR_SET_TIME_CHANGE_DRIVER = 'Не удалось установить время пересменки водителей.';
	const DRIVER_SET_SCHEDULE = 'Не удалось установить график водителя.';
	const DRIVER_SET_WORK = 'Не удалось установить рабочее состояние водителя.';
	const DRIVER_ADD_NODE = 'Не удалось добавить коментарии для водителя.';
	const DRIVER_EDIT_NODE = 'Не удалось отредактировать коментарии для водителя.';
	const DRIVER_REMOVE_NODE = 'Не удалось удалить коментарии для водителя.';
	const DRIVER_SET_STATE = 'Не удалось установить состояние водителя.';
	const MONTH_SET_DAY_VALUE = 'Не удалось установить значение графика водителя.';
	const MONTH_CANCELLATION_REPLACEMENT_DRIVER = 'Не удалось отменить назначеного волителя на замену';
	const DRIVER_TIME_TO_SLEEP = 'Не удалось изменить время сна водителя';
	const DRIVER_INRERCESSION = 'Не удалось добавить примечание для водителя';
	const AUTOMATIC_SCHEDULE_DRIVER = 'Нет графика в начале месяца';
	const NO_RECORDS_SPOOFING = 'Нет записи подмены';
	const ERROR_ADD_RECORD_SUBSTITUTION = 'Ошибка добавления записи';
	const ERROR_DELETE_RECORD_SUBSTITUTION = 'Ошибка удаление записи';
	const ERROR_UPDATE_RECORD_SUBSTITUTION = 'Ошибка изменения записи';
	const ERROR_UPDATE_RECORD_SUBSTITUTION_MONTHS = 'Ошибка изменения записей в месяце';
	const ERROR_UPDATE_SEQUENCE_DRIVERS_ON_CARS = 'Не удалось изменить последовательность водителей на машине';
	const test = 'test epta';
}
?>