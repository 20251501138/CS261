# Noun Analysis — UPSC Civil Services Examination & Merit Management System

This document contains the candidate noun extraction, elimination filter analysis, and final surviving domain classes derived from our three use case specifications (UC-01, UC-06, and UC-09).

---

## 1. Raw Candidate Noun List

The following candidate nouns and noun phrases were identified from UC-01, UC-06, and UC-09:

Candidate, Examination, Portal, Account, Registration ID, Profile, Confirmation Message, E-mail, Mobile Number, Exam Cycle, Personal Details, Name, Date of Birth, Gender, Category, Password, Security Question, Format, One-Time Password (OTP), Registration Session, Examiner, Evaluator, Exam Administrator, Answer Sheet, Answer Script, Batch, Evaluation Batch, Marking Scheme, Model Answers, Queue, Question, Marks, Permissible Range, Progress, Status, Re-scan Request, Merit List, Reservation Policy, Cut-off Marks, Stage, Session, Set, Tie-Breaking Rule, Discrepancy Remark, Notification System.

---

## 2. Candidate Filtering Analysis

Each candidate noun is systematically evaluated against the four standard Object-Oriented Analysis (OOA) elimination filters:
1. **Redundancy:** Synonyms representing identical concepts already modeled.
2. **Attribute / Primitive Data:** Simple data fields belonging inside an entity rather than forming an independent class.
3. **Vagueness / Irrelevance:** UI layers, external boundaries, or terms outside core domain logic.
4. **Operation / Event / Transient:** Actions, temporary validation tokens, or fleeting runtime events.

### Filtered Nouns Table

| Candidate Noun | Decision | Filter Applied | Reason / Belongs To |
| :--- | :--- | :--- | :--- |
| **Candidate** | **Survives** | — | Primary user entity in the examination lifecycle. |
| **Exam Administrator** | **Survives** | — | Administrative user managing verification, allocations, and approvals. |
| **Examiner** | **Survives** | — | User entity responsible for grading answer scripts. |
| **Answer Script** | **Survives** | — | Domain entity storing masked script data, question marks, and lock state. |
| **Evaluation Batch** | **Survives** | — | Aggregates and tracks collections of scripts for examiner queues. |
| **Merit List** | **Survives** | — | Manages aggregate scoring, ranking, and approval state. |
| **Reservation Policy** | **Survives** | — | Encapsulates statutory quota percentages and cutoff calculation rules. |
| **Notification System** | **Survives** | — | Service entity handling multi-channel alert delivery. |
| `Evaluator` | Discarded | Redundancy | Duplicate synonym for `Examiner`. |
| `Answer Sheet` | Discarded | Redundancy | Duplicate synonym for `Answer Script`. |
| `Queue` | Discarded | Redundancy | Handled as a collection of `EvaluationBatch` objects inside `Examiner`. |
| `Name`, `Date of Birth`, `Gender` | Discarded | Attribute | Demographic attributes stored inside `Candidate` / `User`. |
| `Category` | Discarded | Attribute | Enum property inside `Candidate`. |
| `Password`, `Security Question` | Discarded | Attribute | Authentication fields inside `User`. |
| `E-mail`, `Mobile Number` | Discarded | Attribute | Contact fields inside `User`. |
| `Registration ID` | Discarded | Attribute | Unique identifier attribute of `Candidate`. |
| `Marks`, `Permissible Range` | Discarded | Attribute | Score array attributes inside `Answer Script`. |
| `Cut-off Marks` | Discarded | Attribute | Numeric threshold value stored in `Merit List`. |
| `Progress`, `Status` | Discarded | Attribute | State enumeration values inside `EvaluationBatch` and `Application`. |
| `Stage`, `Session`, `Set` | Discarded | Attribute | Simple descriptive properties of an examination cycle. |
| `Personal Details` | Discarded | Attribute | Collection of primitive demographic fields in `Candidate`. |
| `Discrepancy Remark` | Discarded | Attribute | String field in verification records. |
| `Portal` | Discarded | Vagueness / Irrelevance | User interface boundary, not a domain entity. |
| `Format` | Discarded | Vagueness / Irrelevance | Input validation rule, not a class. |
| `Account`, `Profile` | Discarded | Vagueness / Irrelevance | Abstract wrappers represented directly by `User` and `Candidate`. |
| `Examination`, `Exam Cycle` | Discarded | Vagueness / Irrelevance | Static configuration context. |
| `Marking Scheme`, `Model Answers` | Discarded | Vagueness / Irrelevance | Reference criteria used by validation logic. |
| `One-Time Password (OTP)` | Discarded | Operation / Transient | Temporary authentication token handled by helper routine. |
| `Registration Session` | Discarded | Operation / Transient | Runtime execution state. |
| `Confirmation Message` | Discarded | Operation / Transient | String payload dispatched through `Notification System`. |
| `Re-scan Request` | Discarded | Operation / Transient | Exception flag raised during batch evaluation. |
| `Tie-Breaking Rule` | Discarded | Operation / Transient | Algorithmic method executed inside `Merit List`. |

---

## 3. Surviving Classes Summary

1. **`User` (Abstract Base Class):** Base class storing common authentication and contact details (`userId`, `fullName`, `email`, `phone`).
2. **`Candidate` (Derived from `User`):** Stores applicant-specific profile data (`registrationNumber`, `category`, `dateOfBirth`).
3. **`ExamAdministrator` (Derived from `User`):** Manages document verifications, batch allocation, and result approvals.
4. **`Examiner` (Derived from `User`):** Evaluates assigned script batches and enters question marks.
5. **`AnswerScript`:** Contains masked script ID, question marks, aggregate score, and evaluation lock state.
6. **`EvaluationBatch`:** Groups answer scripts together and tracks batch completion progress.
7. **`MeritList`:** Handles score compilation, quota enforcement, and rank generation.
8. **`ReservationPolicy`:** Holds reservation category percentages and computes qualifying cutoffs.