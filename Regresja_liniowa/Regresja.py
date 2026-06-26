def linear_regression(points):
    n = len(points)
    sum_x = sum(p[0] for p in points)
    sum_y = sum(p[1] for p in points)
    sum_xy = sum(p[0] * p[1] for p in points)
    sum_x2 = sum(p[0] * p[0] for p in points)

    # Obliczenia zgodnie ze wzorem regresji liniowej
    a = (n * sum_xy - sum_x * sum_y) / (n * sum_x2 - sum_x**2)
    b = (sum_y - a * sum_x) / n
    return a, b


# Pobieranie wejścia od użytkownika
print("Podaj liczbę punktów:")
n = int(input())

punkty = []
for i in range(n):
    print(f"Podaj punkt #{i+1} w formacie: x y")
    x, y = map(float, input().split())
    punkty.append((x, y))

a, b = linear_regression(punkty)

print(f"\nWynik regresji:")
print(f"a = {a:.4f}")
print(f"b = {b:.4f}")
print(f"Równanie prostej: y = {a:.4f}x + {b:.4f}")
