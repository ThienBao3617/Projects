let count = 0;
let matrix = [
  [0, 0, 0, 0],
  [0, 0, 0, 0],
  [0, 0, 0, 0],
  [0, 0, 0, 0],
];
let totalfee = 0;

// Lấy ra các phần tử span thông tin từ HTML
var nameSpan = document.getElementById("nameText");
var ageSpan = document.getElementById("ageText");
var genderSpan = document.getElementById("genderText");
var dobSpan = document.getElementById("dobText");
var addressSpan = document.getElementById("addressText");

// Cập nhật thông tin
nameSpan.textContent = localStorage.getItem("name");
ageSpan.textContent = localStorage.getItem("age");
genderSpan.textContent = localStorage.getItem("gender");
dobSpan.textContent = localStorage.getItem("dob");
addressSpan.textContent = localStorage.getItem("address");

function showSection(section) {
  // Ẩn tất cả các phần
  $(".user-section").hide();
  // Hiển thị phần được chọn
  $("#" + section).show();
}
function nextSection_DatVe() {
  $("#info").hide();
  $("#booking").show();
}
function logout() {
  localStorage.setItem("name", "");
  localStorage.setItem("age", "");
  localStorage.setItem("address", "");
  localStorage.setItem("dob", "");
  localStorage.setItem("gender", "");
  window.location.href = "/";
}

// Lắng nghe sự kiện khi click vào ô đậu xe
document.addEventListener("click", function (event) {
  if (event.target.classList.contains("parking-spot")) {
    event.target.classList.toggle("selected");
    count = document.querySelectorAll(".selected").length;
    calculateTimeDifference();
  }
});

// Tạo ma trận ô vuông
const parkingGrid = document.getElementById("parkingGrid");
for (let i = 0; i < 4; i++) {
  for (let j = 0; j < 4; j++) {
    const spot = document.createElement("div");
    spot.classList.add("parking-spot");
    spot.textContent = `${String.fromCharCode(65 + i)}${"-"}${j + 1}`;
    parkingGrid.appendChild(spot);
    spot.dataset.row = i; // Thêm thuộc tính dữ liệu cho hàng
    spot.dataset.column = j; // Thêm thuộc tính dữ liệu cho cột
    spot.addEventListener("click", handleSpotClick); // Thêm sự kiện click cho mỗi ô
    parkingGrid.appendChild(spot);
  }
}

// Hàm xử lý sự kiện khi nhấp chuột vào ô
function handleSpotClick(event) {
  const clickedSpot = event.target;
  const row = clickedSpot.dataset.row;
  const column = clickedSpot.dataset.column;
  // Sử dụng toán tử ba ngôi để thay đổi giá trị của phần tử từ 0 thành 1 và ngược lại
  matrix[row][column] = matrix[row][column] === 0 ? 1 : 0;
  console.log(matrix);
}

function calculateTimeDifference() {
  const inputStart = document.getElementById("startDateTime").value;
  const inputEnd = document.getElementById("endDateTime").value;

  const startHours = parseInt(inputStart.split(":")[0]);
  const startMinutes = parseInt(inputStart.split(":")[1]);

  const endHours = parseInt(inputEnd.split(":")[0]);
  const endMinutes = parseInt(inputEnd.split(":")[1]);

  const parkingCostSpan = document.getElementById("parkingCost");

  // Calculate the duration in minutes
  const durationInMinutes = calculateDuration(
    startHours,
    startMinutes,
    endHours,
    endMinutes
  );
  // Calculate the total cost
  // Đọc giá trị từ Local Storage
  var cost = localStorage.getItem("cost");
  const totalCost = (durationInMinutes / 60) * cost * count;
  totalfee = totalCost.toFixed(2);
  // Display the total cost
  parkingCostSpan.textContent = totalCost.toFixed(2);
}

function calculateDuration(startHours, startMinutes, endHours, endMinutes) {
  const startDate = new Date(0, 0, 0, startHours, startMinutes); // Tạo đối tượng Date cho thời điểm bắt đầu
  const endDate = new Date(0, 0, 0, endHours, endMinutes); // Tạo đối tượng Date cho thời điểm kết thúc

  let differenceInMinutes = endDate.getTime() - startDate.getTime(); // Tính sự chênh lệch giữa hai thời điểm (đơn vị: mili giây)
  differenceInMinutes = differenceInMinutes / (1000 * 60); // Chuyển đổi sang phút

  // Đảm bảo kết quả không bị âm
  if (differenceInMinutes < 0) {
    differenceInMinutes += 24 * 60; // Thêm 24 giờ vào kết quả nếu bị âm
  }
  return differenceInMinutes;
}

var rowCount = 0; // Biến đếm số lượng hàng đã được thêm vào

document.getElementById("submit").addEventListener("click", function () {
  var name = localStorage.getItem("name");
  var age = localStorage.getItem("age");
  var gender = localStorage.getItem("gender");
  var dob = localStorage.getItem("dob");
  var address = localStorage.getItem("address");
  var paymentMethod = document.getElementById("paymentMethod").value;
  var timeStart = document.getElementById("startDateTime").value;
  var timeEnd = document.getElementById("endDateTime").value;
  // Kiểm tra tính hợp lệ của giá trị
  if (
    !name ||
    !age ||
    !gender ||
    !dob ||
    !address ||
    !paymentMethod ||
    !parkingCost
  ) {
    alert("Vui lòng điền đầy đủ thông tin");
    return;
  }

  rowCount++; // Tăng biến đếm

  // Tạo nội dung mã QR
  var qrContent = `,T:${timeStart + "-" + timeEnd},C:${totalfee},P:${matrix}`;
  // Tạo một hàng mới trong bảng
  var newRow = document.createElement("tr");

  // Tạo các ô dữ liệu cho hàng mới
  var idCell = document.createElement("td");
  idCell.textContent = rowCount;
  newRow.appendChild(idCell);

  var nameCell = document.createElement("td");
  nameCell.textContent = name;
  newRow.appendChild(nameCell);

  var dobCell = document.createElement("td");
  dobCell.textContent = dob;
  newRow.appendChild(dobCell);

  var dateCell = document.createElement("td");
  dateCell.textContent = getTime();
  newRow.appendChild(dateCell);

  var payCell = document.createElement("td");
  payCell.textContent = paymentMethod;
  newRow.appendChild(payCell);

  var qrCell = document.createElement("td");
  var qrDiv = document.createElement("div");
  qrDiv.id = "id-qrcode-" + rowCount;
  qrCell.appendChild(qrDiv);
  newRow.appendChild(qrCell);
  // Thêm hàng mới vào tbody của bảng
  document.querySelector("#bookingHistoryTable tbody").appendChild(newRow);

  // Tạo mã QR bằng thư viện QRCode.js
  var qrcode = new QRCode("id-qrcode-" + rowCount, {
    width: 250,
    height: 250,
  });
  qrcode.makeCode(qrContent);
  alert("Thành công!");
});

function getTime() {
  var currentTime = new Date();

  // Lấy ngày, tháng và năm hiện tại
  var currentDay = currentTime.getDate();
  var currentMonth = currentTime.getMonth() + 1; // Tháng bắt đầu từ 0, nên cần cộng thêm 1
  var currentYear = currentTime.getFullYear();

  // Lấy giờ, phút và giây hiện tại
  var currentHour = currentTime.getHours();
  var currentMinute = currentTime.getMinutes();
  var currentSecond = currentTime.getSeconds();

  // Định dạng lại ngày, tháng và năm thành chuỗi
  var formattedDay = (currentDay < 10 ? "0" : "") + currentDay;
  var formattedMonth = (currentMonth < 10 ? "0" : "") + currentMonth;
  var formattedYear = currentYear;

  // Định dạng lại giờ, phút và giây thành chuỗi
  var formattedHour = (currentHour < 10 ? "0" : "") + currentHour;
  var formattedMinute = (currentMinute < 10 ? "0" : "") + currentMinute;
  var formattedSecond = (currentSecond < 10 ? "0" : "") + currentSecond;

  // Tạo chuỗi thời gian với định dạng mong muốn
  var formattedTime =
    formattedDay +
    "/" +
    formattedMonth +
    "/" +
    formattedYear +
    ", " +
    formattedHour +
    ":" +
    formattedMinute +
    ":" +
    formattedSecond;

  return formattedTime;
}
