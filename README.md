# Chat_v.1.0
Czat zapętlony na głównym wątku. 
Odczytuje połączenie, jeśli jest na głównym gnieździe, to jest to nowe połączenie i zostaje mu przypisany nowe gniazdo.
Jeśli to połączenie na innym gnieździe niż główne, to jest to wiadomość, która zostaje nadana do pozostałych podłączonych klientów.

Klient działa na dwóch wątkach. Jeden nasłuchujący wiadomości przychodzącej, i drugi czekający na mozliwość wysłania.

Nadchodzące zmiany:

1.0 działający serwer na jednym wątku

1.1 możliwość przesyłania plików

2.0 obsługa wielu wątków

2.1 możliwość logowania się

2.1 obsługa bazy danych

3.0 wersja okienkowa

