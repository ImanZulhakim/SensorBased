from flask import Flask, render_template, jsonify
import pymysql
import pymysql.err
import time

app = Flask(__name__)
app.secret_key = "flash_message"

# Configure database connection
config = {
    'user': 'root',
    'password': '',
    'port': 3306,
    'host': 'localhost',
    'database': 'environmental_monitoring'
}

# Global variable to hold last updated timestamp
last_updated = None

@app.route('/')
def index():
    try:
        conn = pymysql.connect(**config)
        cursor = conn.cursor()
        cursor.execute('SELECT * FROM sensor_data ORDER BY id DESC LIMIT 5')  # Fetch the last row
        data = cursor.fetchall()
        cursor.close()
        conn.close()

        # Extract the humidity and light intensity values from the last row
        if len(data) > 0:
            temperature = data[0][1]
            humidity = data[0][2]
            light_intensity = data[0][3]
        else:
            temperature = None
            humidity = None
            light_intensity = None

    except pymysql.MySQLError as e:
        data = []
        humidity = None
        light_intensity = None
        connection_status = f"Failed to connect to database: {e}"

    return render_template('index.html', data=data, temperature=temperature, humidity=humidity,
                           light_intensity=light_intensity)


@app.route('/last_updated')
def get_last_updated():
    global last_updated
    return str(last_updated)


if __name__ == "__main__":
    app.run(host='127.0.0.1', port=500, debug=True) # i use 500 because 5000 is busy
