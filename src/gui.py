from flask import Flask, request, render_template_string
import subprocess

app = Flask(__name__)

HTML = """
<!doctype html>
<html>
<head>
  <title>IR Search GUI</title>
  <style>
    body { font-family: Arial, sans-serif; max-width: 900px; margin: 40px auto; }
    input { width: 70%; padding: 10px; }
    button { padding: 10px 14px; }
    pre { background: #f5f5f5; padding: 14px; border-radius: 8px; white-space: pre-wrap; }
  </style>
</head>
<body>
  <h2>Vector Space Model Search</h2>
  <form method="post">
    <input name="query" placeholder="Enter query..." value="{{ query or '' }}">
    <button type="submit">Search</button>
  </form>

  {% if output %}
    <h3>Result</h3>
    <pre>{{ output }}</pre>
  {% endif %}
</body>
</html>
"""

@app.route("/", methods=["GET", "POST"])
def home():
    output = ""
    q = ""
    if request.method == "POST":
        q = request.form.get("query", "").strip()
        if q:
            try:
                # IMPORTANT: absolute path is safer
                proc = subprocess.run(
                    ["/home/muhammad/Desktop/Fast/Semester 6/IR/assignment-2/src/app", "--query", q],
                    capture_output=True,
                    text=True,
                    timeout=60
                )
                output = proc.stdout if proc.returncode == 0 else proc.stderr
            except Exception as e:
                output = f"Error: {e}"
    return render_template_string(HTML, output=output, query=q)

if __name__ == "__main__":
    app.run(host="127.0.0.1", port=5000, debug=True)