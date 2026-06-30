FROM python:3.11-slim

ENV PYTHONDONTWRITEBYTECODE=1 \
    PYTHONUNBUFFERED=1

RUN apt-get update \
    && apt-get install -y --no-install-recommends ffmpeg libgomp1 libsndfile1 \
    && rm -rf /var/lib/apt/lists/*

RUN pip install --no-cache-dir \
    f5-tts==1.1.20 \
    piper-tts==1.4.2 \
    soundfile==0.13.1 \
    torch==2.8.0 \
    torchaudio==2.8.0

COPY scripts/f5_batch.py /app/f5_batch.py

WORKDIR /workspace
