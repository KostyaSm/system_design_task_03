-- Создание нового пользователя
INSERT INTO users (login, first_name, last_name, email, phone)
VALUES ($1, $2, $3, $4, $5)
RETURNING id;

-- Поиск пользователя по логину
SELECT id, login, first_name, last_name, email, phone, created_at
FROM users
WHERE login = $1;

-- Поиск пользователя по маске имени и фамилии
SELECT id, login, first_name, last_name, email, phone, created_at
FROM users
WHERE (first_name || ' ' || last_name) ILIKE $1
ORDER BY similarity((first_name || ' ' || last_name), $1) DESC;

-- Создание товара
INSERT INTO products (name, description, price, stock_quantity, category)
VALUES ($1, $2, $3, $4, $5)
RETURNING id;

-- Получение списка товаров
SELECT id, name, description, price, stock_quantity, category, created_at
FROM products
WHERE category = COALESCE($1, category)
  AND price <= COALESCE($2::NUMERIC, price)
ORDER BY created_at DESC
LIMIT $3 OFFSET $4;

-- Добавление товара в корзину и обновление количества
INSERT INTO cart_items (user_id, product_id, quantity)
VALUES ($1, $2, $3)
ON CONFLICT (user_id, product_id)
DO UPDATE SET quantity = cart_items.quantity + EXCLUDED.quantity
RETURNING id;

-- Получение корзины для пользователя
SELECT
    ci.id,
    ci.quantity,
    ci.added_at,
    p.id AS product_id,
    p.name AS product_name,
    p.price,
    p.description,
    (p.price * ci.quantity) AS total_price
FROM cart_items ci
JOIN products p ON ci.product_id = p.id
WHERE ci.user_id = $1
ORDER BY ci.added_at DESC;

-- Общая сумма корзины
SELECT SUM(p.price * ci.quantity) AS cart_total
FROM cart_items ci
JOIN products p ON ci.product_id = p.id
WHERE ci.user_id = $1;