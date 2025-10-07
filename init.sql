SELECT 'CREATE DATABASE myapp'
WHERE NOT EXISTS (SELECT FROM pg_database WHERE datname = 'myapp')\gexec

-- Подключаемся к созданной базе
\c myapp
