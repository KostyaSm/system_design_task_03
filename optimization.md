

# Оптимизация запросов и анализ планов выполнения

В данном документе представлены результаты анализа планов выполнения SQL-запросов с использованием команды `EXPLAIN ANALYZE`. Все замеры производились на тестовом наборе данных (10 записей в каждой таблице).

---

## 1. Поиск пользователя по логину

**Запрос:**
```sql
SELECT * FROM users WHERE login = 'ivanov';

Index Scan using idx_users_login on users  (cost=0.14..8.16 rows=1 width=1140) (actual time=0.024..0.025 rows=1 loops=1)
  Index Cond: ((login)::text = 'ivanov'::text)
Planning Time: 0.171 ms
Execution Time: 0.048 ms
```

**Анализ:**
- **Тип сканирования:** Index Scan по индексу idx_users_login.
- **Эффективность:** База данных не читает всю таблицу, а сразу обращается к индексу B-tree.
- **Время выполнения:** 0.048 мс.
- **Вывод:** Индекс работает корректно, обеспечивая мгновенный поиск по уникальному полю.

---

## 2. Поиск пользователя по маске имени и фамилии

**Запрос:**
```sql
SELECT * FROM users WHERE (first_name || ' ' || last_name) ILIKE '%Иван%';

Seq Scan on users  (cost=0.00..11.05 rows=1 width=1140) (actual time=0.017..0.025 rows=1 loops=1)
  Filter: ((((first_name)::text || ' '::text) || (last_name)::text) ~~* '%Иван%'::text)
  Rows Removed by Filter: 9
Planning Time: 0.819 ms
Execution Time: 0.042 ms
```

**Анализ:**
- **Тип сканирования:** Seq Scan (последовательное сканирование).
- **Почему не используется индекс?** Несмотря на наличие GIN-индекса (idx_users_name_pattern на основе pg_trgm), оптимизатор PostgreSQL выбрал полное сканирование таблицы.
- **Причина:** В таблице всего 10 записей. Для такого малого объёма данных стоимость обращения к индексу (чтение дерева индекса + чтение кучи таблицы) выше, чем простое последовательное чтение всех страниц таблицы с диска.
- **Прогноз:** При увеличении таблицы до тысяч записей PostgreSQL автоматически переключится на использование GIN-индекса, так как это станет дешевле.

---

## 3. Получение корзины пользователя (JOIN)

**Запрос:**
```sql
SELECT ci.*, p.name, p.price 
FROM cart_items ci 
JOIN products p ON ci.product_id = p.id 
WHERE ci.user_id = 1;

Hash Join  (cost=14.47..25.61 rows=8 width=556) (actual time=0.049..0.052 rows=2 loops=1)
  Hash Cond: (p.id = ci.product_id)
  ->  Seq Scan on products p  (cost=0.00..10.90 rows=90 width=536) (actual time=0.007..0.008 rows=10 loops=1)
  ->  Hash  (cost=14.37..14.37 rows=8 width=24) (actual time=0.024..0.025 rows=2 loops=1)
        Buckets: 1024  Batches: 1  Memory Usage: 9kB
        ->  Bitmap Heap Scan on cart_items ci  (cost=4.21..14.37 rows=8 width=24) (actual time=0.016..0.017 rows=2 loops=1)
              Recheck Cond: (user_id = 1)
              Heap Blocks: exact=1
              ->  Bitmap Index Scan on idx_cart_user_id  (cost=0.00..4.21 rows=8 width=0) (actual time=0.012..0.012 rows=2 loops=1)
                    Index Cond: (user_id = 1)
Planning Time: 0.702 ms
Execution Time: 0.100 ms
```

**Анализ:**
- **Фильтрация:** Используется Bitmap Index Scan по индексу idx_cart_user_id. База данных быстро находит нужные строки в корзине.
- **Соединение:** Используется Hash Join. PostgreSQL загружает отфильтрованные строки корзины в хэш-таблицу в памяти и быстро соединяет их с таблицей товаров.
- **Вывод:** Индекс на внешнем ключе (user_id) критически важен для быстрого формирования корзины без блокировок.

---

## 4. Фильтрация товаров по категории

**Запрос:**
```sql
SELECT * FROM products WHERE category = 'Электроника';

Index Scan using idx_products_category on products  (cost=0.14..8.16 rows=1 width=798) (actual time=0.091..0.096 rows=6 loops=1)
  Index Cond: ((category)::text = 'Электроника'::text)
Planning Time: 0.143 ms
Execution Time: 0.124 ms
```

**Анализ:**
- **Тип сканирования:** Index Scan по индексу idx_products_category.
- **Эффективность:** Запрос выполняется за 0.124 мс.
- **Вывод:** Индекс позволяет быстро отбирать товары для каталога, не перебирая весь ассортимент.

---

## Итоговая таблица

| Запрос | Тип сканирования | Использованный индекс | Время выполнения |
|--------|------------------|------------------------|------------------|
| Поиск по логину | Index Scan | idx_users_login | 0.048 мс |
| Поиск по маске | Seq Scan (оптимизатор) | idx_users_name_pattern (готов) | 0.042 мс |
| Корзина (JOIN) | Bitmap Index Scan | idx_cart_user_id | 0.100 мс |
| Фильтр по категории | Index Scan | idx_products_category | 0.124 мс |



---

## Сравнение планов выполнения: до и после оптимизации

Для демонстрации влияния индексов на производительность был проведён запрос поиска пользователя по логину.

### Что сделано:
1. Удалён индекс `idx_users_login`
2. Выполнен запрос без индекса
3. Создан индекс заново
4. Выполнен запрос с индексом

### Запрос:
```sql
SELECT * FROM users WHERE login = 'ivanov';
```

### Результат без индекса `idx_users_login`
```
Index Scan using users_login_key on users  (cost=0.14..8.16 rows=1 width=1140) (actual time=0.070..0.071 rows=1 loops=1)
  Index Cond: ((login)::text = 'ivanov'::text)
Planning Time: 0.273 ms
Execution Time: 0.096 ms
```

Даже после удаления индекса `idx_users_login` запрос продолжил использовать индекс `users_login_key`. Это связано с тем, что ограничение `UNIQUE(login)` автоматически создаёт свой индекс для обеспечения уникальности. Таким образом, поле `login` всегда индексируется, независимо от наличия дополнительного индекса.

### Результат с индексом `idx_users_login`
```
Seq Scan on users  (cost=0.00..1.12 rows=1 width=1140) (actual time=0.010..0.012 rows=1 loops=1)
  Filter: ((login)::text = 'ivanov'::text)
  Rows Removed by Filter: 9
Planning Time: 0.238 ms
Execution Time: 0.024 ms
```

После создания индекса оптимизатор выбрал `Seq Scan` (полное сканирование).

**Так произошло потому что**
- В таблице всего 10 записей.
- Стоимость `Seq Scan` (`cost=0.00..1.12`) ниже, чем `Index Scan` (`cost=0.14..8.16`).
- Для малого объёма данных быстрее прочитать всю таблицу подряд, чем обращаться к дереву индекса.

### Таблица: сравнение планов выполнения: до и после оптимизации

| Параметр | Без доп. индекса | С индексом `idx_users_login` |
|----------|-----------------|------------------------------|
| Тип сканирования | Index Scan (через UNIQUE) | Seq Scan (выбор оптимизатора) |
| Cost | 0.14..8.16 | 0.00..1.12 |
| Execution Time | 0.096 ms | 0.024 ms |

**Итог:** PostgreSQL использует Cost-Based Optimizer, который выбирает план выполнения на основе статистики и объёма данных. На малых таблицах (`< 1000 строк`) полное сканирование часто оказывается эффективнее индексного поиска. При росте данных оптимизатор автоматически переключится на `Index Scan`, и преимущество индекса станет очевидным.


