import socket
import threading
import time

# Configuration
SERVER_IP = "127.0.0.1"
SERVER_PORT = 8080
NUM_THREADS = 100  # Nombre de threads
REQUESTS_PER_THREAD = 200  # Nombre de requêtes par thread

# Fonction pour effectuer plusieurs requêtes
def send_requests(thread_id):
    for i in range(REQUESTS_PER_THREAD):
        try:
            # Création de la connexion socket
            with socket.create_connection((SERVER_IP, SERVER_PORT)) as s:
                # Envoi d'une requête GET simple
                s.sendall(b"GET / HTTP/1.1\r\nHost: 127.0.0.1\r\n\r\n")
                # Lecture de la réponse
                response = s.recv(1024)
                # Optionnel : décommenter pour afficher la réponse
                # print(f"Thread {thread_id} - Réponse : {response.decode('utf-8')}")
        except Exception as e:
            print(f"Thread {thread_id} - Erreur : {e}")

# Test principal
def main():
    print(f"Début du test avec {NUM_THREADS} threads, {REQUESTS_PER_THREAD} requêtes par thread.")
    start_time = time.time()

    # Lancer les threads
    threads = []
    for i in range(NUM_THREADS):
        thread = threading.Thread(target=send_requests, args=(i,))
        threads.append(thread)
        thread.start()

    # Attendre que tous les threads se terminent
    for thread in threads:
        thread.join()

    end_time = time.time()
    total_requests = NUM_THREADS * REQUESTS_PER_THREAD
    duration = end_time - start_time
    rps = total_requests / duration

    print(f"\nTest terminé en {duration:.2f} secondes")
    print(f"Total de requêtes : {total_requests}")
    print(f"Requêtes par seconde : {rps:.2f}")

if __name__ == "__main__":
    main()
