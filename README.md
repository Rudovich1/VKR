# Библиотека для имплементации генетического алгоритма на языке C++

Простой инструмент, позволяющий без лишнего труда настроить и запустить генетический алгоритм

## TODO
- codegen c++ -> python
- наполнить functions-store
- общий API
- тесты для store-api
- TUI
- подключение произвольного кода
- рефактор codegen

### TODO...
- store.json -> sql
- GUI

# (УСТАРЕЛО)

## Основные структуры

[Файл types.hpp](genetic_method\types.hpp)

Обертка над произвольным типом, характеризующая ген
```c++
template<typename GeneType>
class Gene
```

Массив генов / особь / хромосома
```c++
template<typename GeneType>
class Chromosome
```

Массив хромосом / поколение - набор хромосом в популяции в некоторой момент времени
```c++
template<typename GeneType>
class Generation
```

Массив поколений / популяция
```c++
template<typename GeneType>
class Population
```

## Интерфейсы

[Файл interfaces.hpp](genetic_method\interfaces.hpp)

### Основные

Фитнес функция / функция приспособленности
```c++
template<typename GeneType, typename ... Args>
struct FitnessFunctionWrapper
{
    virtual std::function<double(const Types::Chromosome<GeneType>&)> operator()(Args& ...) = 0;
};
```
Функция генерации стартового поколения
```c++
template<typename GeneType, typename ... Args>
struct StartGenerationFunctionWrapper
{
    virtual std::function<Types::Generation<GeneType>()> operator()(Args& ...) = 0;
};
```
Функция отбора хромосом в следующее поколение
```c++
template<typename GeneType, typename ... Args>
struct SelectionFunctionWrapper
{
    virtual std::function<void(Types::Generation<GeneType>&)> operator()(Args& ...) = 0;
};
```
Условия остановки эволюции популяции
```c++
template<typename GeneType, typename ... Args>
struct ConditionsForStoppingWrapper
{
    virtual std::function<bool(const Types::Population<GeneType>&)> operator()(Args& ...) = 0;
};
```
Функция мутации
```c++
template<typename GeneType, typename ... Args>
struct MutationFunctionWrapper
{
    virtual std::function<void(Types::Generation<GeneType>&)> operator()(Args& ...) = 0;
};
```
Функция кроссинговера
```c++
template<typename GeneType, typename ... Args>
struct CrossingoverFunctionWrapper
{
    virtual std::function<void(Types::Generation<GeneType>&)> operator()(Args& ...) = 0;
};
```
Функция для получения необходимых данных после эволюции популяции
```c++
template<typename GeneType, typename ResultType, typename ... Args>
struct ResultFunctionWrapper
{
    virtual std::function<ResultType(const Types::Population<GeneType>&)> operator()(Args& ...) = 0;
};
```
### Дополнительные

Произвольная функция, которая выполняется после добавления нового поколения
```c++
template<typename GeneType, typename ... Args>
struct AnyFunctionWrapper
{
    virtual std::function<void(const Types::Population<GeneType>&)> operator()(Args& ...) = 0;
};
```
Функция, которая выполняется после завершения эволюции популяции в [дереве эволюции](#древо-эволюции)
```c++
template<typename GeneType, typename ... Args>
struct EndNodeFunctionWrapper
{
    virtual std::function<void()> operator()(Args& ...) = 0;
};
```
Функция для объединения двух популяций в [дереве эволюции](#древо-эволюции)
```c++
template<typename GeneType, typename ... Args>
struct PoolingPopulationsWrapper
{
    virtual std::function<Types::Population<GeneType>(const Types::Population<GeneType>&, const Types::Population<GeneType>&)> operator()(Args& ...) = 0;
};
```

## Древо эволюции

[Файл evolution_tree.hpp](genetic_method\evolution_tree.hpp)

Дерево эволюции используется для того, чтобы описать каким образом будет проходить процесс эволюции всех популяций.
Для каждой конкретной вершины можно описать собственные интерфейсы.
Несколько популяций можно объединить в одну в процессе эволюции.
Для одной популяции можно последовательно запустить процессы эволюции с разными условиями. 

### Вершины дерева

Базовый класс 
```c++
template<typename GeneType>
class Node
```
Листья дерева эволюции, с которых начинается процесс
```c++
template<typename GeneType>
class PopulationNode: public Node<GeneType>
```
Унарная вершина, принимает на вход одну вершину и производит процесс эволюции над результирующей популяцией
```c++
template<typename GeneType>
class UnaryNode: public Node<GeneType>
```
Бинарная вершина, принимает на вход две вершины, объединяет две результирующие популяции в одну и производит над ней процесс эволюции
```c++
template<typename GeneType>
class BinaryNode: public Node<GeneType>
```

`TODO doc описать конструкторы, как создать дерево`

### Условия эволюции

Структура для хранения основных интерфейсов
```c++
template<typename GeneType>
struct BaseConditions
```

`TODO doc более подробно` 

## General

`TODO doc`

## Примеры

### Простой пример

Пример реализации генетического алгоритма для решения задачи о рюкзаке.
Дерево состоит всего из одной вершины

[Здесь](examples\one-node\functions.hpp) написана логика всех интерфейсов

[Здесь](examples\one-node\main.cpp) использование библиотеки для решения простой задачи о рюкзаке

### Более сложный пример

`TODO doc`

### Логирование

`TODO doc`