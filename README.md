Определен интерфейс ITape для работы с устройством типа лента.
Написан класс Tape, реализующий этот интерфейс и эмулирующий работу с лентой посредством обычного файла (есть функции для сдвига ленты на определенное количество позиций, перемотку в начало и конец ленты, чтение из ячейки и запись в ячейку, на которой находится головка с учетом конфигурационных параметров). Пример файла в tape_in.txt (на каждую ячейку ленты отведено 12 символов - длина максимального числа int + символ на "-" + символ на пробел).
Возможно сконфигурировать через внешний конфигурационный файл config.txt, который читается при запуске приложения, задержки по записи/чтению элемента с ленты, перемотки ленты, и сдвига ленты на одну позицию (ровно в таком порядке).
Файлы временных лент сохраняются в директорию tmp (после использования удаляются).
Написан класс DoSort, реализующий алгоритм сортировки данных с входной ленты на выходную (написано для случая N <= (M / 4)^2).
Консольное приложение принимает на вход имя входного и выходного файлов и производит сортировку. В коде программы заданы длина входной и выходной лент N и размер оперативной памяти в байтах M.
