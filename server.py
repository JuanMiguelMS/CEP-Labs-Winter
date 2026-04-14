from http.server import HTTPServer, SimpleHTTPRequestHandler
import os

class Handler(SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=os.path.dirname(os.path.abspath(__file__)), **kwargs)

if __name__ == "__main__":
    server = HTTPServer(("0.0.0.0", 5000), Handler)
    print("CEP-Labs-Winter running on http://0.0.0.0:5000")
    server.serve_forever()
