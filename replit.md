# CEP-Labs-Winter

## Project Overview
A fresh Replit project cloned from GitHub. Currently serves a static landing page via a Python HTTP server.

## Architecture
- **Server**: Python's built-in `http.server` module serving static files on port 5000
- **Frontend**: Single `index.html` landing page
- **No dependencies** beyond Python's standard library

## Key Files
- `server.py` — Python HTTP server, listens on `0.0.0.0:5000`
- `index.html` — Static landing page
- `README.md` — Project title

## Running the App
The "Start application" workflow runs `python server.py` and serves on port 5000.

## Deployment
Configured for autoscale deployment running `python server.py`.
