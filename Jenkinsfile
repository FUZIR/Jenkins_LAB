// Глобальні змінні, щоб вони були доступні у всіх stage
def testCases = []
def reportContent = new StringBuilder()
def author = ""
def teacherEmail = "k.lipovok@gmail.com" // Пошта викладача

pipeline {
    agent any

    // Залишаємо лише параметр для красивого відображення імені в email та HTML-звіті
    parameters {
        string(name: 'REPO_NAME', defaultValue: 'Lab4_Repo', description: 'Ім’я репозиторію для звіту')
    }

    stages {
        stage('Checkout') {
            steps {
                script {
                    echo "Скачуємо код із публічного репозиторію..."
                    // Ця команда автоматично бере код із посилання, налаштованого у веб-інтерфейсі Jenkins (Pipeline -> SCM)
                    checkout scm
                }
            }
        }

        stage('Build') {
            steps {
                script {
                    sh 'gcc -o main main.c'
                    if (!fileExists('./main')) {
                        error "main не знайдено. Етап Build міг пройти невдало."
                    }
                }
            }
        }

        stage('Define Variant') {
            steps {
                script {
                    def output = sh(script: "echo dummy | ./main", returnStdout: true).trim()
                    def outputLines = output.split('\n')
                    def variantLine = outputLines[0].trim()
                    def variant = variantLine as Integer

                    if (variant == 1) {
                        testCases = [
                            ['input': 'hello', 'expected': 'HeLlO'],
                            ['input': '123456', 'expected': '123456'],
                            ['input': 'Hello world 123', 'expected': 'HeLlO WoRlD 123'],
                            ['input': '*/1-==1', 'expected': '*/1-==1']
                        ]
                    } else if (variant == 2) {
                        testCases = [
                            ['input': 'hello', 'expected': 'HELLO'],
                            ['input': '123456', 'expected': '123456'],
                            ['input': 'Hello world 123', 'expected': 'HELLO WORLD 123'],
                            ['input': '*/1-==1', 'expected': '*/1-==1']
                        ]
                    } else {
                        error "Unsupported variant: ${variant}"
                    }
                }
            }
        }

        stage('Form report') {
            steps {
                script {
                    // Збір метаданих з Git
                    def commitHash = sh(script: "git rev-parse HEAD", returnStdout: true).trim()
                    author = sh(script: "git log -1 --pretty=format:'%an'", returnStdout: true).trim()
                    def commitMessage = sh(script: "git log -1 --pretty=format:'%s'", returnStdout: true).trim()
                    def commitTime = sh(script: "git log -1 --pretty=format:'%ci'", returnStdout: true).trim()

                    // Формування HTML шаблону
                    reportContent.append("<html><head><style>table { width: 100%; border-collapse: collapse; } th, td { border: 1px solid black; padding: 8px; text-align: center; } .passed { background-color: lightgreen; } .failed { background-color: lightcoral; }</style></head><body>")
                    reportContent.append("<h2>Test report</h2>")
                    reportContent.append("<p><b>Repository:</b> ${params.REPO_NAME}</p>")
                    reportContent.append("<p><b>Commit:</b> ${commitHash}</p>")
                    reportContent.append("<p><b>Author:</b> ${author}</p>")
                    reportContent.append("<p><b>Commit message:</b> ${commitMessage}</p>")
                    reportContent.append("<p><b>Commit time:</b> ${commitTime}</p>")
                    reportContent.append("<table><tr><th>Input</th><th>Expected</th><th>Output</th><th>Test status</th></tr>")
                }
            }
        }

        stage('Test') {
            steps {
                script {
                    testCases.each { testCase ->
                        // Передаємо вхідний рядок у програму
                        def testOutput = sh(script: "echo ${testCase.input} | ./main", returnStdout: true).trim().split('\n')
                        def result = testOutput[1].trim() // Беремо другий рядок (перший - номер варіанту)
                        def passed = (result == testCase.expected)

                        // Додаємо рядок результатів у таблицю
                        reportContent.append("<tr>")
                        reportContent.append("<td>${testCase.input}</td>")
                        reportContent.append("<td>${testCase.expected}</td>")
                        reportContent.append("<td>${result}</td>")
                        reportContent.append("<td class='${passed ? "passed" : "failed"}'>${passed ? "Passed" : "Failed"}</td>")
                        reportContent.append("</tr>")

                        if (!passed) {
                            reportContent.append("</table></body></html>")
                            writeFile file: 'report.html', text: reportContent.toString()
                            error "Test failed: was expected '${testCase.expected}', but got '${result}'"
                        }
                    }
                    // Якщо всі тести пройдені успішно, закриваємо теги
                    reportContent.append("</table></body></html>")
                    writeFile file: 'report.html', text: reportContent.toString()
                }
            }
        }
    }

    post {
        always {
            // Публікація HTML-звіту у Jenkins
            publishHTML([
                allowMissing: false,
                alwaysLinkToLastBuild: true,
                keepAll: true,
                reportDir: '.',
                reportFiles: 'report.html',
                reportName: 'Test Report'
            ])

            script {
                // Генерація PDF і відправка на пошту
                sh 'wkhtmltopdf report.html report.pdf'
                emailext (
                    subject: "Test report for ${params.REPO_NAME}",
                    body: """<p>PDF Test report is attached.</p>
                             <p>Repository: ${params.REPO_NAME}</p>
                             <p>Author: ${author}</p>""",
                    to: "${teacherEmail}",
                    mimeType: 'text/html',
                    attachmentsPattern: "report.pdf"
                )
            }
        }
    }
}
