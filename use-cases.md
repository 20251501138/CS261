# Use Cases — UPSC Civil Services Examination & Merit Management System

## Actors

| Actor | Type | Description |
|---|---|---|
| **Candidate** | Primary (human) | A person who registers for the exam, submits the application, pays the fee, downloads the admit card, appears for the exam and interview, and checks results/merit status. |
| **Exam Administrator** (UPSC Official) | Primary (human) | Manages the entire exam lifecycle: verifies documents, generates admit cards, oversees evaluation, generates and approves the merit list, schedules interviews, and publishes results. |
| **Examiner / Evaluator** | Primary (human) | Evaluates candidates' answer sheets against the marking scheme and handles re-evaluation of flagged scripts. |
| **Interview Board Member** | Primary (human) | Conducts the personality test / interview for candidates shortlisted after the Mains examination. |
| **Payment Gateway** | Secondary (external system) | Processes the candidate's online application-fee payment and returns a payment confirmation to the system. |
| **Aadhaar Verification System** | Secondary (external system) | Verifies a candidate's identity and demographic details submitted during document verification. |
| **Notification System** | Secondary (external system) | Sends SMS and e-mail alerts to candidates at key stages (registration, admit card, result, interview call). |
| **Scheduler** | Secondary (time-triggered) | Automatically initiates system actions (such as admit-card generation) on pre-configured dates without human intervention. |

The use case diagram (`docs/use-case-diagram.puml`, rendered at
`docs/use-case-diagram.png`) shows all eight actors, the system boundary,
and thirteen use cases, including two `<<include>>` relationships
("Submit Application Form" includes "Make Payment"; "Publish Final Result"
includes "Send Notification") and one `<<extend>>` relationship
("Re-evaluate Answer Sheet" extends "Evaluate Answer Sheets", guarded by
`[candidate has requested re-evaluation within 15 days of result
declaration]`).

---

## UC-01 — Register for Examination

| Field | Detail |
|---|---|
| **Use Case ID** | UC-01 |
| **Primary Actor** | Candidate |
| **Stakeholders and Interests** | • Candidate — wants a quick, error-free way to create an account and start the application before the deadline.<br>• Exam Administrator (UPSC) — wants only genuine, uniquely identifiable candidates registered, to prevent duplicate or fraudulent entries.<br>• System Administrator — wants registration data captured accurately for use by all downstream processes. |
| **Preconditions** | • The registration window for the exam cycle is open.<br>• The candidate has a valid, active e-mail address and mobile number.<br>• The candidate is not already registered for the same exam cycle. |
| **Postconditions (Success Guarantee)** | • A unique Registration ID is created and stored against the candidate's profile.<br>• A confirmation message is sent to the candidate's e-mail and mobile number.<br>• The candidate's account is activated and ready to proceed to application submission. |
| **Trigger** | The candidate clicks "New Registration" on the examination portal within the notified registration window. |

**Main Flow (Basic Path)**
1. Candidate selects "Register for Examination" for the desired exam cycle.
2. System displays the registration form.
3. Candidate enters personal details (name, date of birth, gender, category, e-mail, mobile number).
4. System validates the format of the entered data.
5. Candidate sets a password and a security question.
6. System checks that the e-mail/mobile number is not already registered for this exam cycle.
7. System generates a unique Registration ID.
8. System sends a one-time password (OTP) to the candidate's mobile number and e-mail.
9. Candidate enters the OTP to confirm the details.
10. System activates the account and displays the Registration ID to the candidate.

**Alternate Flows**

*A1 — Duplicate registration detected (at step 6):*
- System finds that the e-mail/mobile number is already registered for this exam cycle.
- System displays an "already registered" message with a link to "Forgot Password / Login".
- Use case ends without creating a new registration.

*A2 — Invalid data format (at step 4):*
- System highlights the invalid field(s) with an explanatory message.
- Candidate corrects the data and resubmits.
- Flow resumes at step 4.

*A3 — OTP expired or incorrect (at step 9):*
- System displays an error and offers a "Resend OTP" option.
- Candidate requests a new OTP and re-enters it.
- Flow resumes at step 9; after three failed attempts the registration session is terminated and the candidate must restart from step 1.

---

## UC-06 — Evaluate Answer Sheets

| Field | Detail |
|---|---|
| **Use Case ID** | UC-06 |
| **Primary Actor** | Examiner / Evaluator |
| **Stakeholders and Interests** | • Candidate — wants a fair, accurate and timely evaluation of their answer script.<br>• Exam Administrator (UPSC) — wants evaluation completed within the schedule while preserving anonymity and integrity of the process.<br>• Examiner — wants an unambiguous marking scheme and a smooth digital tool to record marks. |
| **Preconditions** | • The examination has been conducted and answer sheets have been scanned/digitised.<br>• The Examiner has been allotted a valid batch of anonymised (roll-number masked) answer scripts.<br>• The marking scheme / model answers for the paper have been finalised and uploaded to the system. |
| **Postconditions (Success Guarantee)** | • Marks for every answer script in the batch are recorded against the masked roll number.<br>• The evaluation status of the batch is updated to "Completed".<br>• The recorded scores are locked and made available for merit-list computation. |
| **Trigger** | The Exam Administrator releases a batch of digitised answer sheets into the Examiner's evaluation queue. |

**Main Flow (Basic Path)**
1. Examiner logs into the evaluation module.
2. System displays the list of answer-script batches allotted to the Examiner.
3. Examiner selects a batch to begin evaluation.
4. System displays the first anonymised answer script together with the marking scheme.
5. Examiner awards marks question-wise as per the marking scheme.
6. System validates that the marks entered are within the permissible range for each question.
7. Examiner submits the marks for the current script.
8. System saves the marks and loads the next script in the batch.
9. Steps 5–8 repeat until every script in the batch has been evaluated.
10. Examiner submits the completed batch for finalisation.
11. System locks the batch, updates its status to "Completed" and notifies the Exam Administrator.

**Alternate Flows**

*A1 — Marks entered out of the permissible range (at step 6):*
- System rejects the entry and displays the valid mark range for that question.
- Examiner re-enters the marks.
- Flow resumes at step 6.

*A2 — Examiner pauses evaluation mid-batch:*
- Examiner saves progress and exits before completing all scripts in the batch.
- System stores the batch as "In Progress" with the last saved script recorded.
- Examiner later resumes at step 3, continuing from the next un-evaluated script.

*A3 — Answer script found unreadable or corrupted (at step 4):*
- Examiner flags the script as "unreadable".
- System excludes it from the batch count and raises a re-scan request to the Exam Administrator.
- Flow resumes at step 8 with the next script in the batch.

**Note — Extension point "Re-evaluation":** after the result is declared, if
a candidate applies for re-evaluation of a specific script within 15 days of
the result declaration, this use case is re-invoked for that script through
the `<<extend>>` use case "Re-evaluate Answer Sheet", guarded by the
condition `[candidate has requested re-evaluation within 15 days of result
declaration]`.

---

## UC-09 — Generate Merit List

| Field | Detail |
|---|---|
| **Use Case ID** | UC-09 |
| **Primary Actor** | Exam Administrator |
| **Stakeholders and Interests** | • Candidate — wants an accurate, transparent ranking computed strictly from marks and applicable reservation rules.<br>• UPSC — wants a legally defensible, policy-compliant merit list that can withstand scrutiny/appeals.<br>• Government / Department of Personnel and Training — wants the list to correctly reflect the reservation and cut-off policy for recruitment. |
| **Preconditions** | • All answer-script batches for the exam stage (Prelims / Mains / Interview) have evaluation status "Completed".<br>• Category and reservation data for each candidate have been verified.<br>• The cut-off / qualifying-marks policy for the stage has been finalised by UPSC. |
| **Postconditions (Success Guarantee)** | • A rank-wise merit list (overall and category-wise) is generated and stored.<br>• Category-wise cut-off marks for the stage are computed and recorded.<br>• The merit list is marked "Pending Approval" and made available to the Exam Administrator for review. |
| **Trigger** | The Exam Administrator initiates "Generate Merit List" once every evaluation batch for the stage is marked "Completed". |

**Main Flow (Basic Path)**
1. Exam Administrator selects the exam stage and clicks "Generate Merit List".
2. System checks that all answer-script batches for the stage are in "Completed" status.
3. System aggregates marks for every candidate for the selected stage.
4. System applies score normalisation rules, where applicable, across different exam sessions/sets.
5. System sorts candidates in descending order of aggregate marks, both overall and category-wise.
6. System applies the reservation policy and computes category-wise cut-off marks.
7. System displays the draft merit list with computed ranks to the Exam Administrator for review.
8. Exam Administrator reviews and approves the draft merit list.
9. System stores the approved merit list and marks it ready for publishing.

**Alternate Flows**

*A1 — Pending evaluation batches found (at step 2):*
- System finds one or more batches not yet marked "Completed".
- System displays the list of pending batches and the Examiners responsible.
- Use case ends; Exam Administrator follows up with the concerned Examiner(s) before retrying.

*A2 — Tie in aggregate marks (at step 5):*
- System finds two or more candidates with identical aggregate marks.
- System applies the predefined tie-breaking rule (e.g., marks in the optional subject, then age).
- Ranking continues with the tie resolved.

*A3 — Exam Administrator rejects the draft list (at step 8):*
- Administrator flags a discrepancy, e.g. an incorrect category tag for a candidate.
- System routes the flagged record back for correction.
- Flow resumes at step 3 once the correction has been made.
