import qrcode
import firebase_admin
from firebase_admin import credentials
from firebase_admin import db
import time

# Initialize Firebase
cred_obj = firebase_admin.credentials.Certificate(
    r'D:\Github\Projects\Uni\share\qruserdatabase-firebase-adminsdk-4iw0k-517a8db948.json')
default_app = firebase_admin.initialize_app(cred_obj, {
    'databaseURL': "https://qruserdatabase-default-rtdb.asia-southeast1.firebasedatabase.app/"
})

# Get the ID from your Firebase database
id_ref = db.reference('-NuXuuBDuWA0kSrRCQxB')
id_value = id_ref.get()

# Generate QR code
qr = qrcode.QRCode(
    version=1,
    error_correction=qrcode.constants.ERROR_CORRECT_L,
    box_size=10,
    border=3,
)
qr.add_data(id_value)
qr.make(fit=True)
qr_img = qr.make_image(fill_color="black", back_color="white")

# Save the QR code image
qr_img.save("qr_code.png")

# Display the QR code
qr_img.show()

# Wait for the image to be captured
time.sleep(5)
