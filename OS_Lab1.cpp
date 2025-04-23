/* Shortest Job Next scheduling */
#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <fstream>
using namespace std;

struct Task
{
	char name;			// имя задачи
	int activation;		// время активации задачи, то есть перехода в режим готовности
	int duration;		// длительность выполнения задачи на процессоре
	int	reserved;		// заказное время обслуживания
};

bool compareActivation(Task& a, Task& b)		// функция сравнения задач по времени активации
{
	return (a.activation < b.activation);
}

bool compareReservedTime(Task& a, Task& b)		// функция сравнения задач по заказному времени обслуживания
{
	return a.reserved < b.reserved;
}

void printInfo(int timer, Task& current, vector<Task> ready_to_execute)
{
	cout << setw(3) << timer << setw(3) << '|' << setw(5) << current.name << setw(5) << '|' << setw(7);
	for (int i = 0; i < ready_to_execute.size(); i++)
		cout << ready_to_execute[i].name << " ";
	cout << endl;
}

void getTasks(vector<Task>& tasks)			// чтение задач из файла
{
	ifstream file("test3.txt");
	if (!file.is_open())
	{
		cout << "Ошибка открытия файла" << endl;
		return;
	}

	Task t;
	while (file >> t.name >> t.activation >> t.duration >> t.reserved)
		tasks.push_back(t);

	file.close();
	return;
}

int main()
{
	setlocale(LC_ALL, "Russian");

	vector<Task> tasks;
	getTasks(tasks);

	sort(tasks.begin(), tasks.end(), compareActivation);		// сортировка задач по времени активации

	vector<Task> ready_to_execute;			// очередь готовности
	Task in_progress;						// текущая задача

	bool processor_is_empty = true;			// вначале процессор свободен
	int end = 0;							// время завершения задачи
	int timer = 1;							// счетчик тактов

	cout << setw(6) << "Такт|" << "Процессор|" << setw(5) << "Очередь готовности" << endl;
	
	// пока есть задачи к выполнению
	while (!tasks.empty() || !ready_to_execute.empty())
	{
		// если такт == время конца задачи, то процессор освобождается
		if (timer == end)
			processor_is_empty = true;

		// все задачи со временем активации == timer добавляются в очередь готовности
		// поскольку задачи отсортированы по времени активации, то можно использовать while

		if (!tasks.empty())
		{
			int i = 0;
			while (i < tasks.size() && (tasks[i].activation == timer))
			{
				ready_to_execute.push_back(tasks[i]);
				i++;
			}
			// удаляем эти задачи из вектора
			tasks.erase(tasks.begin(), tasks.begin() + i);
		}
		
		// если процессор свободен, то находим минимальную по длительности задачу
		// и удаляем ее из очереди готовности
		if (processor_is_empty)
		{
			auto min = min_element(ready_to_execute.begin(), ready_to_execute.end(), compareReservedTime);
			in_progress = *min;						// запоминаем текущую задачу
			int index = distance(ready_to_execute.begin(), min);
			ready_to_execute.erase(ready_to_execute.begin() + index);
					
			processor_is_empty = false;				// процессор теперь занят
			end = timer + in_progress.duration;		// вычисляем время завершения задачи
		}

		printInfo(timer, in_progress, ready_to_execute);

		timer++;
	}

	// пока последняя задача не завершена, выводим информацию
	while (timer != end)
	{
		printInfo(timer, in_progress, ready_to_execute);
		timer++;
	}
	cout << setw(3) << timer << setw(3) << '|' << " все задачи выполнены" << endl;

	return 0;
}