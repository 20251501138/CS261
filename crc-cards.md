# CRC Cards — UPSC Civil Services Examination & Merit Management System

This document contains the Class-Responsibility-Collaboration (CRC) cards for all surviving domain classes identified during the Object-Oriented Analysis (OOA) phase.

---

### Class: User (Abstract Base Class)

| Responsibilities | Collaborators |
| :--- | :--- |
| • Maintain core authentication details (`userId`, `passwordHash`).<br>• Store personal contact details (`fullName`, `email`, `phone`).<br>• Authenticate login credentials.<br>• Provide polymorphic base interface for role-specific dashboards. | *None* |

---

### Class: Candidate (inherits from User)

| Responsibilities | Collaborators |
| :--- | :--- |
| • Maintain candidate profile (`registrationNumber`, `category`, `dateOfBirth`).<br>• Submit application for an active examination cycle.<br>• Request answer script re-evaluation within permitted window.<br>• Access admit card and view published merit list status. | `AnswerScript`<br>`NotificationService` |

---

### Class: ExamAdministrator (inherits from User)

| Responsibilities | Collaborators |
| :--- | :--- |
| • Maintain administrator credentials (`employeeId`).<br>• Verify candidate uploaded documents and flag discrepancies.<br>• Allocate evaluation batches to assigned examiners.<br>• Review and approve stage-wise draft merit lists.<br>• Authorize publication of final examination results. | `EvaluationBatch`<br>`MeritList` |

---

### Class: Examiner (inherits from User)

| Responsibilities | Collaborators |
| :--- | :--- |
| • Maintain examiner profile (`examinerCode`, `specialization`).<br>• Access assigned queue of anonymized script batches.<br>• Grade scripts question-wise against permissible mark limits.<br>• Finalize and submit completed batches. | `EvaluationBatch`<br>`AnswerScript` |

---

### Class: AnswerScript

| Responsibilities | Collaborators |
| :--- | :--- |
| • Encapsulate masked script ID (`maskedScriptId`) to preserve candidate anonymity.<br>• Store subject details and question-wise marks array.<br>• Compute aggregate total score.<br>• Enforce evaluation lock to prevent tampering once grading is finalized. | `Examiner` |

---

### Class: EvaluationBatch

| Responsibilities | Collaborators |
| :--- | :--- |
| • Group collections of anonymized answer scripts (`batchId`, `subject`).<br>• Track evaluation progress state (`status`: Assigned, In Progress, Completed).<br>• Lock all contained scripts upon batch completion. | `AnswerScript`<br>`Examiner`<br>`ExamAdministrator` |

---

### Class: MeritList

| Responsibilities | Collaborators |
| :--- | :--- |
| • Consolidate aggregate candidate scores across examination stages.<br>• Apply category-wise reservation rules and compute cut-offs.<br>• Execute tie-breaking logic and compute descending ranks.<br>• Maintain approval and publication status. | `Candidate`<br>`ReservationPolicy`<br>`ExamAdministrator` |

---

### Class: ReservationPolicy

| Responsibilities | Collaborators |
| :--- | :--- |
| • Define statutory category quota percentages (EWS, OBC, SC, ST, PwBD).<br>• Compute stage-wise qualifying cutoffs per category based on vacancy distribution. | `MeritList` |

---

### Class: NotificationService

| Responsibilities | Collaborators |
| :--- | :--- |
| • Format and dispatch milestone alert messages via SMS and Email.<br>• Deliver automated alerts for registration OTP, admit card availability, and result declaration. | `User` |