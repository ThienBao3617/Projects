function Person(name, age, dob, address, gender, user, password) {
  this.name = name;
  this.age = age;
  this.dob = dob;
  this.address = address;
  this.gender = gender;
  this.user = user;
  this.password = password;
}
let arrayOfObjects = [];
let person1 = new Person(
  "Tuan",
  18,
  "18-03-2006",
  "HCM",
  "Nam",
  "user1",
  "user1"
);
arrayOfObjects.push(person1);

function showLoginForm(role) {
  document.getElementById("loginForm").style.display = "block";
  document.getElementById("username").value = "";
  document.getElementById("password").value = "";
  document.getElementById("message").innerText = "";
  document.getElementById("loginForm").setAttribute("data-role", role);
  document.getElementById("registerForm").style.display = "none";
}
function login() {
  var username = document.getElementById("username").value;
  var password = document.getElementById("password").value;
  var role = document.getElementById("loginForm").getAttribute("data-role");
  if (role === "user") {
    let foundMatch = false;
    arrayOfObjects.forEach(function (person) {
      // Kiểm tra xem chuỗi nhập vào có trùng với username và userpassword của person không
      if (username === person.user && password === person.password) {
        // Lưu giá trị vào Local Storage
        localStorage.setItem("name", `${person.name}`);
        localStorage.setItem("age", `${person.age}`);
        localStorage.setItem("address", `${person.address}`);
        localStorage.setItem("dob", `${person.dob}`);
        localStorage.setItem("gender", `${person.gender}`);
        foundMatch = true;
        window.location.href = "./template/user.html";
        return; // Dừng vòng lặp nếu tìm thấy trùng khớp
      }
    });
    // Nếu không tìm thấy trùng khớp sau khi duyệt qua tất cả các đối tượng
    if ((foundMatch = false)) {
      alert("Đăng nhập Không Thành công!");
    }
  } else if (role === "admin") {
    console.log("aaa");
    if (username === "admin" && password === "admin") {
      window.location.href = "./template/admin.html";
    } else {
      console.log("assa");
    }
  }
}
function showRegisterForm() {
  document.getElementById("registerForm").style.display = "block";
  document.getElementById("regUsername").value = "";
  document.getElementById("regName").value = "";
  document.getElementById("regPassword").value = "";
  document.getElementById("regAge").value = "";
  document.getElementById("regDob").value = "";
  document.getElementById("regGender").value = "";
  document.getElementById("regAddress").value = "";
  document.getElementById("loginForm").style.display = "none";
}

function register() {
  let useName = document.getElementById("regUsername").value;
  let password = document.getElementById("regPassword").value;
  let name = document.getElementById("regName").value;
  let age = document.getElementById("regAge").value;
  let dob = document.getElementById("regDob").value;
  let gender = document.getElementById("regGender").value;
  let address = document.getElementById("regAddress").value;
  arrayOfObjects.push(
    new Person(name, age, dob, address, gender, useName, password)
  );
  console.log(arrayOfObjects);
  alert("Đăng ký Thành công!");
}
