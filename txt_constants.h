#ifndef TXT_CONSTANTS_H
#define TXT_CONSTANTS_H

#include <QString>

//  MainWindow    -------------------------------------
const QString TITLE_WINDOW = "Планирование графика работы водителей v 1.1.2";
const QString TXT_ERROR = "Ошибка";
const QString TXT_FILE = "Файл";
const QString TXT_SETTING = "Правка";
const QString TXT_EXPORT = "Экспорт";
const QString TXT_UPDATE_AUTOPARK = "Обновить";
const QString TXT_UPDATE = "Обновить автопарк";
const QString TXT_EXIT = "Выход";
const QString TXT_USERS = "Пользователи";
const QString TXT_CONNECTION = "Соединение";
const QString TXT_USING_CARS = "Использование автомобилей";
const QString TXT_ALL_AUTO = "Все авто";
const QString TXT_DRIVERS_STAFF = "Штат";
const QString MSG_CLOSE_WINDOW_EXIST_REQUESTS = "Не все данные отправлены или получены от сервера,\nожидать окончания?";

const QString TXT_WAYBILLS = "Путевые листы";
const QString TXT_WAYBILLS_PRINT = "Печать путевых листов";
//  end MainWindow    ---------------------------------

//  TableControl    -----------------------------------
const QString TXT_ID = "ИД";
const QString TXT_CAR = "Автомобиль";
const QString TXT_DRIVER = "Водитель";
const QString TXT_CAR_MODEL = "Марка, модель";
const QString TXT_CAR_NUMBER = "Номер";
const QString TXT_TIME_CHANGE_DRIVER = "Время пересменки";
const QString TXT_DRIVER_PHONE = "Телефон";
const QString TXT_DRIVER_FIO = "ФИО";
const QString TXT_DRIVER_COUNT_TRUANCY = "Прогулы";
const QString TXT_DRIVER_SCHEDULE = "График";
const QString TXT_DRIVER_SLEEP = "Сон";
const QString TXT_DRIVER_INTERCESSION = "Примечание";
const QString TXT_REMOME = "Удалить";
const QString TXT_PUT_DOWN_THE_SCHEDULE = "Проставить график";
const QString TXT_PUT_DOWN_THE_SCHEDULE_FOR_THE_NEXT_MONTH = "График на сл. месяц";
const QString TXT_SET_WORK = "Работает";
const QString TXT_SET_NOWORK = "Не работает";
const QString TXT_ADD_DRIVER = "Добавить водителя";
const QString TXT_ADD_REPLACEMENT_DRIVER = "Добавить подменного водителя";
const QString TXT_CAR_TO_REPAIR = "На ремонте";
const QString TXT_CAR_OUT_REPAIR = "Вывести с ремонта";
const QString TXT_CAR_REMOVE = "Удалить";
const QString TXT_NO = "нет";
const QString TXT_OUTPUT = "вых.";
const QString TXT_NOT_GET_THROUGH = "не доз.";
const QString TXT_EXTRA_CHENGE = "доп. см.";
const QString TXT_NOTIFIED = "увед.";
const QString TXT_REPLACEMENT = "замена";
const QString TXT_TRUNCY = "прогул";
const QString TXT_ATTENDANCE = "вышел";
const QString TXT_EDIT = "Изменить";
const QString TXT_ALL = "Все";
const QString TXT_ADD = "Добавить";
const QString MSG_ADD_HAS_ADDED_DRIVER = "Хотите удалить водителя из автомобиля \"[car]\" \"[number]\"\nи добавить к текущему?";
const QString TXT_QUESTION = "Вопрос";
const QString TXT_HOLIDAY = "Отпуск";
const QString TXT_ILL = "Заболел";
const QString TXT_UNDEFINED_SCHEDULE = "Не определенный график";
const QString TXT_NOT_SET = "Не назначен";
const QString TXT_PRIVATE_TRADERS = "Частники";
const QString TXT_NOT_CONTACTED = "Не на связи";
//  end TableControl    -------------------------------

//  LoginWidget    ------------------------------------
const QString TXT_LOGIN = "Логин";
const QString TXT_PASSWORD = "Пароль";
const QString TXT_PARK = "Парк";
const QString TXT_INPUT = "Войти";
const QString TXT_CLOSE = "Закрыть";
//  end LoginWidget    --------------------------------

//  UserDialog    -------------------------------------
const QString TXT_FULL = "Все";
const QString TXT_ADMIN = "Админ";
const QString TXT_READONLY = "Чтение";
const QString TXT_PLANNER = "Планировщик";
const QString TXT_ACCESS = "Права";
const QString MSG_LOGIN_LENGTH = "Логин слишком короткий";
const QString MSG_PASS_LENGTH = "Пароль слишком короткий";
const QString TITLE_USER_DIALOG = "Пользователи";
//  end UserDialog    ---------------------------------

//  DriversDialog     ---------------------------------
const QString TITLE_DRIVERS_DIALOG = "Водители";
//  end DriversDialog ---------------------------------

//  DayToolTipEditDialog     --------------------------
const QString TITLE_DAY_TOOL_TIP_EDIT_DIALOG = "Комментарий";
//  end DayToolTipEditDialog --------------------------

//  DriverNodesDialog     -----------------------------
const QString TITLE_DRIVER_NODES_DIALOG = "Комментарии";
const QString TXT_EDIT_NODE = "Редактировать";
const QString TXT_REMOVE_NODE = "Удалить";
//  end DriverNodesDialog -----------------------------

//  Request    ----------------------------------------
const QString TXT_QUERY_OK = "\"status\":\"1\"";
const QString TXT_ERR_TITLE = "error_msg";
const QString MSG_ERROR_SERVER = "На сервере произошла ошибка.\nПовторите попытку познее.";
const QString MSG_INVALID_RESPONSE = "Получен не коректный ответ от сервера.\nПовторите попытку познее.";
const QString MSG_NO_RESPONSE_IS_RECIVED = "Не получен ответ от сервера.\nПовторите попытку познее.";
//  end Request    ------------------------------------

//  ExcelExport    ------------------------------------
const QString TXT_PLAN_TOMORROW_WORK = "ПЛАН ИСПОЛЬЗОВАНИЯ АВТОМОБИЛЕЙ НА ";
//  end ExcelExport    --------------------------------

// CalendarDialog  ------------------------------------
const QString TITLE_CALENDAR_DIALOG = "Выберите день";
// end CalendarDialog  --------------------------------

//  ScheduleEditWidget  --------------------------------
const QString TXT_SCHEDULE_1_1 = "1/1";
const QString TXT_SCHEDULE_1_2 = "1/2";
const QString TXT_SCHEDULE_1_3 = "1/3";
const QString TXT_SCHEDULE_2_1 = "2/1";
const QString TXT_SCHEDULE_2_2 = "2/2";
const QString TXT_SCHEDULE_3_1 = "3/1";
const QString TXT_SCHEDULE_3_3 = "3/3";
const QString TXT_SCHEDULE_5_2 = "5/2";
const QString TXT_SCHEDULE_6_1 = "6/1";
const QString TXT_SCHEDULE_12d = "12д";
const QString TXT_SCHEDULE_12n = "12н";
//  end ScheduleEditWidget  ----------------------------

// Version ---------------------------------------------
const QString TXT_VERSION = "1.1.2";
const int INT_VERSION = 15;
// end version -----------------------------------------
#endif // TXT_CONSTANTS_H
