# Chat_v.1.0

Serwer: MainServer.cpp

Klient: TempClient.cpp

Czat zapętlony na głównym wątku. 
Odczytuje zmiany na zestawie gniazd, jeśli połączenie jest na głównym gnieździe, to jest to nowe połączenie i zostaje mu przypisane nowe gniazdo.
Jeśli to połączenie na innym gnieździe niż główne, to jest to wiadomość, która zostaje nadana do pozostałych podłączonych klientów.

Klient działa na dwóch wątkach. Jeden nasłuchujący wiadomości przychodzącej, i drugi czekający na mozliwość wysłania.

Nadchodzące zmiany:

1.0 działający serwer na jednym wątku (Zrobione)

1.1 możliwość przesyłania plików

1.2 testy jednostkowe

2.0 obsługa wielu wątków

2.1 możliwość logowania się

2.1 obsługa bazy danych

3.0 wersja okienkowa

