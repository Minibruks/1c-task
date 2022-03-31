Программа позволяет определять на черно белом рисунке количество пересечений в графе.
Для считывания картинки я написал скрипт на Python [read_image.py](/read_image.py) и использую его вывод для считывания rgb пикселей в исходный массив в [исходной программе](/intersect_count.cpp).
Сам алгоритм:
прохожусь по каждому пикселю, если он черный, то дальше в каждую сторону иду только один раз, таким образом толщина линии уменьшается с 3 до 1 и уже можно вычислять вершины и пересечения.
Затем пытаюсь вычислить сами вершины: рассматриваю всевозможные тройки вершин (за вершину мы здесь принимаем пиксель) и ищем те, которые не лежат на одной прямо1 - в таком случае считаем одну из этих точек вершиной.

Затем во вложенных циклах пробегаемся по всем парам вершин и ищем пересечение отрезков.

Из улучшений, которые я бы добавил, но не успел из-за ограничения по времени:

Возможность пользовательского ввода

Оличать вершины от просто пересекающихся линий под прямым углом